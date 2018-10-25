#!/usr/bin/env rspec

require_relative "../test_helper"

require "yast2/clients/view_anymsg"

describe Yast::ViewAnymsgClient do
  describe "#main" do
    before do
      # UI mockup
      allow(Yast::UI).to receive(:OpenDialog)
      allow(Yast::UI).to receive(:ChangeWidget)
      allow(Yast::UI).to receive(:QueryWidget)
      allow(Yast::UI).to receive(:UserInput).and_return(:ok)
      allow(Yast::UI).to receive(:CloseDialog)

      # SCR mock
      allow(Yast::SCR).to receive(:Execute)
      allow(Yast::SCR).to receive(:Read)
      allow(Yast::SCR).to receive(:Write)

      # WFM mock
      allow(Yast::WFM).to receive(:Args).and_return([])

      allow(Yast::FileUtils).to receive(:GetSize).and_return(1)
    end

    it "returns true" do
      expect(subject.main).to eq true
    end

    context "filenames list does not exist yet" do
      it "copies filenames list from ydata dir to var dir" do
        allow(Yast::FileUtils).to receive(:Exists).and_return(false)
        expect(Yast::SCR).to receive(:Execute).with(
          path(".target.bash"),
          "/bin/cp /usr/share/YaST2/data/filenames /var/lib/YaST2/filenames"
        )

        subject.main
      end
    end

    it "shows help text for command line if help parameter is passed" do
      allow(Yast::WFM).to receive(:Args).with(no_args).and_return(["help"])
      allow(Yast::WFM).to receive(:Args).with(0).and_return("help")

      expect(Yast::CommandLine).to receive(:Run).and_return(true)

      subject.main
    end

    context "filenames list" do
      def combobox_items(&block)
        expect(Yast::UI).to receive(:OpenDialog) do |opts, term|
          items = term.nested_find do |i|
            i.is_a?(::Array) && i.first.is_a?(Yast::Term) && i.first.value == :item
          end

          expect(items).to(be_a(Array), "Not found ComboBox items #{term.inspect}")

          block.call(items)
        end
      end

      def expect_to_include(*filenames)
        combobox_items do |items|
          filenames.each do |path|
            result = items.find do |i|
              i.value == :item && i.params.include?(path)
            end

            expect(result).to_not(eq(nil), "Not found path '#{path}' in Items - #{items.inspect}")
          end
        end
      end

      it "reads filenames list from var dir" do
        expect(Yast::SCR).to receive(:Read).with(path(".target.string"), "/var/lib/YaST2/filenames")
          .and_return("")

        subject.main
      end

      it "uses empty list if read failed" do
        expect(Yast::SCR).to receive(:Read).with(path(".target.string"), "/var/lib/YaST2/filenames")
          .and_return(nil)

        subject.main
      end

      it "merge list with default files" do
        expect_to_include(*Yast::ViewAnymsgClient::DEFAULT_FILENAMES)

        subject.main
      end

      it "adds to ComboBox all file from filenames list" do
        filenames = ["/tmp/test", "/tmp/lest"]
        expect(Yast::SCR).to receive(:Read).with(path(".target.string"), "/var/lib/YaST2/filenames")
          .and_return(filenames.join("\n"))
        expect_to_include(*filenames)

        subject.main
      end

      it "skips lines with # at the beginning" do
        filenames = ["/tmp/test", "#/tmp/lest"]

        combobox_items do |items|
          items.each do |item|
            value = item.params[1]
            expect(value).to_not match /lest/
          end
        end

        expect(Yast::SCR).to receive(:Read).with(path(".target.string"), "/var/lib/YaST2/filenames")
          .and_return(filenames.join("\n"))

        subject.main
      end

      it "strips the leading * and mark item as default" do
        filenames = ["/tmp/test", "*/tmp/lest"]
        expect(Yast::SCR).to receive(:Read).with(path(".target.string"), "/var/lib/YaST2/filenames")
          .and_return(filenames.join("\n"))

        combobox_items do |items|
          res = items.find do |item|
            item.params[1] == "/tmp/lest"
          end

          expect(res).to_not(be_nil, "Not found /tmp/lest item in #{items.inspect}")
          expect(res.params[2]).to(eq(true), "Item is not marked as default #{items.inspect}")
        end

        subject.main
      end

      it "writes new default to filenames list" do
        allow(Yast::UI).to receive(:UserInput).and_return(:custom_file, :ok)
        allow(Yast::UI).to receive(:QueryWidget).with(Id(:custom_file), :Value)
          .and_return("/var/log/boot.log")

        expect(Yast::SCR).to receive(:Write) do |scr_path, filepath, content|
          expect(scr_path).to eq path(".target.string")
          expect(filepath).to eq "/var/lib/YaST2/filenames"
          expect(content).to include("*/var/log/boot.log")
        end

        subject.main
      end

      it "writes new custom file to filenames list" do
        allow(Yast::UI).to receive(:UserInput).and_return(:custom_file, :ok)
        allow(Yast::UI).to receive(:QueryWidget).with(Id(:custom_file), :Value)
          .and_return("/var/log/not_seen_before.log")

        expect(Yast::SCR).to receive(:Write) do |scr_path, filepath, content|
          expect(scr_path).to eq path(".target.string")
          expect(filepath).to eq "/var/lib/YaST2/filenames"
          expect(content).to include("*/var/log/not_seen_before.log")
        end

        subject.main

      end

      it "does not write anything when user click cancel" do
        allow(Yast::UI).to receive(:UserInput).and_return(:cancel)

        expect(Yast::SCR).to_not receive(:Write)

        subject.main
      end
    end
  end
end
