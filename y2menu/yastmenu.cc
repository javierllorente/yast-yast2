/*
  Textdomain "base"
*/

#include <locale.h>
#include <string>
using std::string;

#include "yastfunc.h"
#include "yast.h"
#include "parseyast.h"
#include "myintl.h"

static int max_mod_width, box_x = 2, box_y, menu_width, tag_x, item_x;
static int mod_width, mod_tag_x, mod_item_x, max_mod_dsp;

static void
print_sec_item (WINDOW * win, string tag, const char *item,
		int choice, int selected)
{
  /* Clear 'residue' of last item */
/*  wattrset (win, selected ? tag_selected_attr : menubox_attr);*/
  wattrset (win, 0);
  wmove (win, choice, 0);
  for (int i = 0; i < menu_width; i++)
    waddch (win, (selected ? tag_selected_attr : menubox_attr) | ' ');
  wmove (win, choice, tag_x);
/*    wattrset (win, selected ? tag_key_selected_attr : tag_key_attr);
    waddch (win, tag[0]); */
  wattrset (win, selected ? tag_selected_attr : tag_attr);
  waddstr (win, tag.c_str ());
  wmove (win, choice, item_x);
  wattrset (win, selected ? tag_selected_attr : tag_attr);
  waddstr (win, item);
  wnoutrefresh (win);
}

static void
print_help_item (WINDOW * win, string tag, const char *item,
		 int choice, int selected, int width)
{
  wattrset (win, 0);
  wmove (win, choice, 0);
  for (int i = 0; i < width; i++)
    waddch (win, (selected ? tag_selected_attr : menubox_attr) | ' ');
  wmove (win, choice, tag_x);
  wattrset (win, selected ? tag_selected_attr : tag_attr);
  waddstr (win, tag.c_str ());
  wmove (win, choice, item_x);
  wattrset (win, selected ? tag_selected_attr : tag_attr);
  waddstr (win, item);
  wnoutrefresh (win);
}

static void
print_mod_item (WINDOW * win, string tag, const char *item,
		int choice, int selected)
{
  /* Clear 'residue' of last item */
  wattrset (win, selected ? tag_selected_attr : menubox_attr);
  wmove (win, choice, 0);
  wattrset (win, selected ? tag_selected_attr : menubox_attr);
  for (int i = 0; i < mod_width; i++)
    waddch (win, ' ');
  wmove (win, choice, mod_tag_x);
  wattrset (win, selected ? tag_selected_attr : tag_attr);
/*    wattrset (win, selected ? tag_key_selected_attr : tag_key_attr);
    waddch (win, tag[0]); */

  waddstr (win, tag.c_str ());
  wmove (win, choice, mod_item_x);
  wattrset (win, selected ? tag_selected_attr : tag_attr);
  waddstr (win, item);
}

/* ------------------------------------------- */

void
yast_draw_box (WINDOW * win, int y, int x, int height, int width,
	       chtype box, chtype border)
{
  wattrset (win, 0);
  for (int i = 0; i < height; i++)
   {
     wmove (win, y + i, x);
     for (int j = 0; j < width; j++)
       if (!i && !j)
	 waddch (win, border | ACS_ULCORNER);
       else if (i == height - 1 && !j)
	 waddch (win, border | ACS_LLCORNER);
       else if (!i && j == width - 1)
	 waddch (win, border | ACS_URCORNER);
       else if (i == height - 1 && j == width - 1)
	 waddch (win, border | ACS_LRCORNER);
       else if (!i)
	 waddch (win, border | ACS_HLINE);
       else if (i == height - 1)
	 waddch (win, border | ACS_HLINE);
       else if (!j)
	 waddch (win, border | ACS_VLINE);
       else if (j == width - 1)
	 waddch (win, border | ACS_VLINE);
       else
	 waddch (win, border | ' ');
   }
}

void
yast_print_button (WINDOW * win, const char *label, int y, int x,
		   int selected)
{
  int i, temp;

  wmove (win, y, x);
  wattrset (win, selected ? button_active_attr : button_inactive_attr);
  waddstr (win, "[");
  temp = strspn (label, " ");
  mouse_mkbutton (y, x, strlen (label) + 2, tolower (label[temp]));
  label += temp;
  wattrset (win, selected ? button_label_active_attr
	    : button_label_inactive_attr);
  for (i = 0; i < temp; i++)
    waddch (win, ' ');
  wattrset (win, selected ? button_key_active_attr
	    : button_key_inactive_attr);
  waddch (win, label[0]);
  wattrset (win, selected ? button_label_active_attr
	    : button_label_inactive_attr);
  waddstr (win, label + 1);
  wattrset (win, selected ? button_active_attr : button_inactive_attr);
  waddstr (win, "]");
  wmove (win, y, x + temp + 1);
}

void yast_title (WINDOW * win, int width, string title)
{
  wattrset (win, title_attr);
  wmove (win, 0, 0);
  waddch (win, ' ');
  waddstr (win, title.c_str ());
  waddch (win, ' ');
}

WINDOW *yast_newwin (int height, int width, int y, int x, int boxflag)
{
  WINDOW *dialog;

  dialog = newwin (height, width, y, x);

  mouse_setbase (x, y);
  keypad (dialog, TRUE);

  if (boxflag)
    yast_draw_box (dialog, 1, 0, height - 1, width, dialog_attr, dialog_attr);
  return dialog;
}

void
yast_mod_menu (WINDOW * dialog, WINDOW * menu, int section, int width,
	       int height)
{
  int i, item_no = modules[section].size ();
  int max_choice = MIN (max_mod_dsp, item_no);

  for (i = 0; i < item_no; i++)
   {
     modules[section][i].textstr.resize (max_mod_width - 2);
     mod_tag_x = MAX (mod_tag_x, modules[section][i].textstr.size () + 2);
     mod_item_x = MAX (mod_item_x, 1);
   }

/*    mod_tag_x = (width - mod_tag_x) / 2; */
  mod_tag_x = 1;
  mod_item_x = mod_tag_x + mod_item_x + 2;

  /* Print the menu */

  for (i = 0; i < max_choice; i++)
    print_mod_item (menu, modules[section][i].textstr, "",
		    i, yast_mod_auto ? FALSE : i == 0);
  wnoutrefresh (menu);

  if (height < item_no)
   {
     wattrset (dialog, darrow_attr);
     wmove (dialog, INFO_Y - 1, box_x + menu_width + 1 + (mod_width / 2));
     waddch (dialog, ACS_DARROW);
//     wmove (dialog, box_y + height + 1, box_x + mod_tag_x + 2);
     wmove (dialog, INFO_Y - 1, 2 + box_x + menu_width + (mod_width / 2));
     waddstr (dialog, "(+)");
   }

  wrefresh (dialog);
  /* register the new window, along with its borders */
}

void print_mod_info (WINDOW * dialog, string info)
{
  int i;
  string tmpstr1, tmpstr2;

  if (info.size () > (2 * (COLS - 7)))
    info.resize (2 * (COLS - 7));
  for (string::iterator i = info.begin (); i != info.end (); i++)
    if (*i == '\n')
      *i = ' ';

  wattrset (dialog, dialog_attr);
  wmove (dialog, INFO_Y, 1);
  for (i = 1; i < COLS - 1; i++)
    waddch (dialog, ' ');
  wmove (dialog, INFO_Y + 1, 1);
  for (i = 1; i < COLS - 1; i++)
    waddch (dialog, ' ');
  if (info.size () > (COLS - 5))
   {
     for (i = (COLS - 5); i > 0 && info[i] != ' '; i--);
     tmpstr1 = string (info, 0, i);
     tmpstr2 = string (info, i + 1);
   }

  if (tmpstr1.size () > 0)
   {
     wmove (dialog, INFO_Y, 3);
     waddstr (dialog, tmpstr1.c_str ());
     wmove (dialog, INFO_Y + 1, 3);
     waddstr (dialog, tmpstr2.c_str ());
   }
  else
   {
     wmove (dialog, INFO_Y, 3);
     waddstr (dialog, info.c_str ());
   }

}

int
mod_nav (WINDOW * dialog, WINDOW * menu, int section, int box_x, int box_y,
	 int key)
{
  int menu_height = max_mod_dsp;

  /*MIN (INFO_Y-box_y, modules[section].size ()); */
  int item_no = modules[section].size ();
  int i;
  int cur_x, cur_y;
  int max_choice = MIN (max_mod_dsp, modules[section].size ());
  static int choice = 0, scroll = 0;

/* dirty hack to re-init the static variables */
  if (key == DEHIGHLIGHT)
   {
     print_mod_item (menu, modules[section][scroll + choice].textstr, "",
		     choice, FALSE);
     if (scroll > 0)
     {
	 wattrset (dialog, darrow_attr);
	 wmove (dialog, box_y - 1, box_x + menu_width + 1);
	 waddch (dialog, ACS_UARROW);
	 wmove (dialog, box_y - 1, box_x + menu_width + 2);
	 waddstr (dialog, "(-)");
     }
     if (menu_height + scroll < item_no)
      {
	wattrset (dialog, darrow_attr);
	wmove (dialog, INFO_Y - 1, box_x + menu_width + 1);
	waddch (dialog, ACS_DARROW);
	wmove (dialog, INFO_Y - 1, box_x + menu_width + 2);
	waddstr (dialog, "(+)");
      }
     wnoutrefresh (menu);
     return 0;
   }

  if (key == HIGHLIGHT)
   {
     print_mod_item (menu, modules[section][scroll + choice].textstr, "",
		     choice, TRUE);
      if (scroll > 0)
     {
	 wattrset (dialog, darrow_attr);
	 wmove (dialog, box_y - 1, box_x + menu_width + 1);
	 waddch (dialog, ACS_UARROW);
	 wmove (dialog, box_y - 1, box_x + menu_width + 2);
	 waddstr (dialog, "(-)");
     }
     if (menu_height + scroll < item_no)
      {
	wattrset (dialog, darrow_attr);
	wmove (dialog, INFO_Y - 1, box_x + menu_width + 1);
	waddch (dialog, ACS_DARROW);
	wmove (dialog, INFO_Y - 1, box_x + menu_width + 2);
	waddstr (dialog, "(+)");
      }
     wnoutrefresh (menu);
     return 0;
   }

  if (key == NAV_INIT)
   {
     scroll = 0;
     choice = 0;
     print_mod_item (menu, modules[section][0].textstr, "", 0, TRUE);
     print_mod_info (dialog, modules[section][scroll + choice].infostr);
     if (menu_height < item_no)
      {
	wattrset (dialog, darrow_attr);
	wmove (dialog, INFO_Y - 1, box_x + menu_width + 1);
	waddch (dialog, ACS_DARROW);
	wmove (dialog, INFO_Y - 1, box_x + menu_width + 2);
	waddstr (dialog, "(+)");
      }

     wnoutrefresh (menu);
     return 0;
   }

  if (key == NAV_RESET)
   {
     print_mod_item (menu, modules[section][scroll + choice].textstr,
		     "", choice, FALSE);
     print_mod_info (dialog, "");
     if (scroll > 0)
     {
	 wattrset (dialog, darrow_attr);
	 wmove (dialog, box_y - 1, box_x + menu_width + 1);
	 waddch (dialog, ACS_UARROW);
	 wmove (dialog, box_y - 1, box_x + menu_width + 2);
	 waddstr (dialog, "(-)");
     }
     if (menu_height < item_no - scroll)
     {
	 wattrset (dialog, darrow_attr);
	 wmove (dialog, INFO_Y - 1, box_x + menu_width + 1);
	 waddch (dialog, ACS_DARROW);
	 wmove (dialog, INFO_Y - 1, box_x + menu_width + 2);
	 waddstr (dialog, "(+)");
     }
     wnoutrefresh (menu);
     wrefresh (dialog);
     return 0;
   }

  if (key == '\n')
   {
     string tmpstr;

     tmpstr = string ("yast ") + modules[section][scroll + choice].name +
       " " + modules[section][scroll + choice].args;

     set_textdomain ("base");
     print_mod_info (dialog, _( "Loading Module..." ));
     wrefresh (dialog);
     system (tmpstr.c_str ());
     wrefresh (dialog);
     return 2;
   }
  /* Check if key pressed matches first character of any
     item tag in menu */
/*  for (i = 0; i < max_choice; i++)
    if (toupper (key) == toupper (modules[section][(scroll + i)].textstr[0]))
      break;
      */

/* forward check */
  for (i = scroll + choice + 1; i < modules[section].size (); i++)
    if (toupper (key) == toupper (modules[section][i].textstr[0]))
     {
       int newi = 0;

       while (scroll + choice != i)
	{

	  print_mod_item (menu, modules[section][scroll + choice].textstr,
			  "", choice, FALSE);
	  if (scroll + max_choice < modules[section].size ())
	   {
	     scrollok (menu, TRUE);
	     wscrl (menu, 1);
	     scrollok (menu, FALSE);
	     scroll++;
	   }
	  else
	    choice++;

	}			/* while */
       print_mod_item (menu, modules[section][scroll + choice].textstr,
		       "", choice, TRUE);
       print_mod_info (dialog, modules[section][scroll].infostr);
       if (choice < max_choice)
	 for (newi = choice + 1; newi < max_choice; newi++)
	  {
	    print_mod_item (menu, modules[section][scroll + newi].textstr,
			    "", newi, FALSE);
	  }

       wnoutrefresh (menu);
       return 1;
     }

  /* backward check */
  for (i = 0; i < scroll + choice; i++)
    if (toupper (key) == toupper (modules[section][i].textstr[0]))
     {
       int newi = 0;

       print_mod_item (menu, modules[section][scroll + choice].textstr,
		       "", choice, FALSE);
       scrollok (menu, TRUE);
       wscrl (menu, scroll * -1);
       scrollok (menu, FALSE);
       scroll = 0;
       choice = 0;
       while (scroll + choice != i)
	{
	  print_mod_item (menu, modules[section][scroll + choice].textstr,
			  "", choice, FALSE);
	  if (scroll + max_choice < modules[section].size ())
	   {
	     scrollok (menu, TRUE);
	     wscrl (menu, 1);
	     scrollok (menu, FALSE);
	     scroll++;
	   }
	  else
	    choice++;

	}			/* while */
       print_mod_item (menu, modules[section][scroll + choice].textstr,
		       "", choice, TRUE);
       print_mod_info (dialog, modules[section][scroll].infostr);
       if (choice < max_choice)
	 for (newi = choice + 1; newi < max_choice; newi++)
	   print_mod_item (menu, modules[section][scroll + newi].textstr,
			   "", newi, FALSE);

       wnoutrefresh (menu);
       return 1;
     }

  /* ---------- */

  if ((key >= '1' && key <= MIN ('9', '0' + max_choice)) ||
      key == KEY_UP || key == KEY_DOWN || key == '-' ||
      key == '+' || (key >= M_EVENT && key - M_EVENT < ' '))
   {
     if (key >= '1' && key <= MIN ('9', '0' + max_choice))
       i = key - '1';
     else if (key >= M_EVENT)
       i = key - M_EVENT;
     else if (key == KEY_UP || key == '-')
      {
	if (!choice)
	 {
	   if (scroll)
	    {
	      /* Scroll menu down */
	      getyx (dialog, cur_y, cur_x);
	      if (menu_height > 1)
	       {
		 /* De-highlight current first item */
		 print_mod_item (menu, modules[section][scroll].textstr,
				 "", 0, FALSE);

		 scrollok (menu, TRUE);
		 wscrl (menu, -1);
		 scrollok (menu, FALSE);
	       }
	      scroll--;
	      print_mod_item (menu, modules[section][scroll].textstr,
			      "", 0, TRUE);
	      print_mod_info (dialog, modules[section][scroll].infostr);
	      wnoutrefresh (menu);

	      /* print the up/down arrows */
	      wmove (dialog, box_y - 1, box_x + menu_width + 1);
	      wattrset (dialog, scroll ? uarrow_attr : menubox_attr);
	      waddch (dialog, scroll ? ACS_UARROW : ACS_HLINE);
	      wmove (dialog, box_y - 1, box_x + menu_width + 2);
	      waddch (dialog, scroll ? '(' : ACS_HLINE);
	      wmove (dialog, box_y - 1, box_x + menu_width + 3);
	      waddch (dialog, scroll ? '-' : ACS_HLINE);
	      wmove (dialog, box_y - 1, box_x + menu_width + 4);
	      waddch (dialog, scroll ? ')' : ACS_HLINE);
	      wattrset (dialog, darrow_attr);
	      wmove (dialog, box_y - 1 + menu_height + 1,
		     box_x + menu_width + 1);
	      waddch (dialog, ACS_DARROW);
	      wmove (dialog, box_y - 1 + menu_height + 1,
		     box_x + menu_width + 2);
	      waddstr (dialog, "(+)");
	      wmove (dialog, cur_y, cur_x);
	      wrefresh (dialog);
	    }
	   return 1;
	 }
	else
	  i = choice - 1;
      }
     else
      {
	if ((key == KEY_DOWN || key == '+'))
	 {
	   if (choice == max_choice - 1)
	    {
	      if (scroll + choice < item_no - 1)
	       {
		 /* Scroll menu up */
		 getyx (dialog, cur_y, cur_x);
		 if (menu_height > 1)
		  {
		    /* De-highlight current last item */
		    print_mod_item (menu,
				    modules[section][scroll + max_choice -
						     1].textstr, "",
				    max_choice - 1, FALSE);
		    scrollok (menu, TRUE);
		    scroll (menu);
		    scrollok (menu, FALSE);
		  }
		 scroll++;
		 print_mod_item (menu,
				 modules[section][scroll + max_choice -
						  1].textstr, "",
				 max_choice - 1, TRUE);
		 print_mod_info (dialog,
				 modules[section][scroll + max_choice -
						  1].infostr);
		 wnoutrefresh (menu);

		 /* print the up/down arrows */
		 wattrset (dialog, uarrow_attr);
		 wmove (dialog, box_y - 1, box_x + menu_width + 1);
		 waddch (dialog, ACS_UARROW);
		 wmove (dialog, box_y - 1, box_x + menu_width + 2);
		 waddstr (dialog, "(-)");
		 wmove (dialog, box_y - 1 + menu_height + 1,
			box_x + menu_width + 1);
		 wattrset (dialog, (scroll + choice < item_no - 1) ?
			   darrow_attr : menubox_attr);
		 waddch (dialog, scroll + choice < item_no - 1 ?
			 ACS_DARROW : ACS_HLINE);
		 wmove (dialog, box_y - 1 + menu_height + 1,
			box_x + menu_width + 2);
		 waddch (dialog, scroll + choice < item_no - 1 ?
			 '(' : ACS_HLINE);
		 wmove (dialog, box_y - 1 + menu_height + 1,
			box_x + menu_width + 3);
		 waddch (dialog, scroll + choice < item_no - 1 ?
			 '+' : ACS_HLINE);
		 wmove (dialog, box_y - 1 + menu_height + 1,
			box_x + menu_width + 4);
		 waddch (dialog, scroll + choice < item_no - 1 ?
			 ')' : ACS_HLINE);
		 wmove (dialog, cur_y, cur_x);
		 wrefresh (dialog);
	       }
	      return 1;
	    }
	   else
	     i = choice + 1;
	 }			/* if */
      }				/* else */

     if (i != choice)
      {
	/* De-highlight current item */
	getyx (dialog, cur_y, cur_x);	/* Save cursor position */
	print_mod_item (menu, modules[section][scroll + choice].textstr,
			"", choice, FALSE);

	/* Highlight new item */
	choice = i;
	print_mod_item (menu, modules[section][scroll + choice].textstr,
			"", choice, TRUE);
	print_mod_info (dialog, modules[section][scroll + choice].infostr);
	wnoutrefresh (menu);
	wmove (dialog, cur_y, cur_x);
	wrefresh (dialog);
      }
     return 1;
   }

  return 0;

}				/* nav */

int yast_menu (const char *title, const char *prompt, int height, int width)
{
  int keep_going = 1;
  int mod_height;
  int mod_menu_on = 0;
  int i, x, y, cur_x, cur_y;
  int key = 0;
  static int button = BUTTON_OK, oldchoice=1,choice = 0, scroll = 0;
  int max_choice = MIN (MAX_GRP_DSP, groups.size ());
  int menu_height = max_choice;
  WINDOW *dialog, *menu, *mod_menu = NULL;
  int item_no = groups.size ();
  int button_x = 0, button_y = 0;
  char hostname[BUFSIZ];
  string titlestr;

  hostname[0] = 0;
  gethostname (hostname, BUFSIZ);
  titlestr = string (YAST_TITLE) + string (hostname);
  titlestr.resize (COLS - 2, ' ');

  x = 0;
  y = 0;

  dialog = yast_newwin (LINES, COLS, y, x, 1);

/*    wattrset (dialog, dialog_attr);
    wmove (dialog, LINES - 3, 0);
    waddch (dialog, ACS_LTEE); */

/*    for (i = 0; i < (COLS-1) - 2; i++)
	waddch (dialog, ACS_HLINE);
  wattrset (dialog, dialog_attr);
  waddch (dialog, ACS_RTEE);*/

  /* top line 3 */
  wattrset (dialog, dialog_attr);
  wmove (dialog, y + 3, 0);
  waddch (dialog, ACS_LTEE);

  for (i = 0; i < COLS - 2; i++)
    waddch (dialog, ACS_HLINE);
  wattrset (dialog, dialog_attr);
  waddch (dialog, ACS_RTEE);

  wmove (dialog, (LINES - 1) - 2, 1);

  for (i = 0; i < (COLS - 1) - 2; i++)
    waddch (dialog, ' ');

  yast_title (dialog, width, titlestr);
  yast_print_autowrap (dialog, prompt, (COLS - 1) - 2, 2, 3, check_attr);

  menu_width = width - 6;
  for (i = 0; i < item_no; i++)
   {
     tag_x = MAX (tag_x, groups[i].textstr.size () + 2);
     item_x = MAX (item_x, groups[i].textstr.size ());
   }

  menu_width = tag_x + 3;
/* set mod_height, mod_width */
  mod_height =
    MIN (INFO_Y - (box_y + choice) - 2, modules[scroll + choice].size ());;
  mod_width = COLS - (menu_width + box_x + x + 10);

  getyx (dialog, cur_y, cur_x);
  box_y = y + GROUP_Y;
/*    box_x = (width - menu_width) / 2 - 1  + GROUP_X; */
  box_x = GROUP_X;

  /* create new window for the menu */
  menu = subwin (dialog, menu_height, menu_width, y + box_y + 1,
		 x + box_x + 1);

  wrefresh (dialog);
  keypad (menu, TRUE);

  /* draw a box around the menu items */
  yast_draw_box (dialog, box_y, box_x, menu_height + 2, menu_width + 2,
		 menubox_border_attr, menubox_attr);
  yast_grp_menu (dialog, menu, choice, menu_width, menu_height);

  button_x = COLS / 2 - 11;
  button_y = (LINES - 1) - 2;
  yast_print_button (dialog, button_help, button_y, 3,
		     (button == BUTTON_HELP) ? TRUE : FALSE);
  yast_print_button (dialog, button_cancel, button_y,
		     (COLS - 5) - strlen (button_cancel),
		     (button == BUTTON_CANCEL) ? TRUE : FALSE);

  wrefresh (dialog);

  max_mod_width = (COLS - 5) - (box_x + menu_width + 2);
  /* auto-open module frame */
  if (yast_mod_auto && modules[scroll + choice].size () > 0)
   {
     int max_items =
       MIN (INFO_Y - (box_y + choice) - 2, modules[scroll + choice].size ());
     int i, max_len = 0, tmplen = 0;

     max_mod_dsp = max_items;
     mod_height = max_mod_dsp;
     for (i = 0; i < modules[scroll + choice].size (); i++)
       if ((tmplen = modules[scroll + choice][i].textstr.size ()) > max_len)
	 mod_width = max_len =
	   yast_dyn_mod ? MIN (max_mod_width, tmplen + 2) : max_mod_width;

     print_sec_item (menu, groups[scroll + choice].textstr,
		     "->", choice, TRUE);
     wnoutrefresh (menu);
     mod_menu =
       subwin (dialog, mod_height, mod_width, box_y + choice + y + 1,
	       3 + x + box_x + menu_width);
     yast_draw_box (dialog, box_y + choice, 2 + box_x + menu_width,
		    max_items + 2, mod_width + 2, menubox_border_attr,
		    inputbox_attr);
     yast_mod_menu (dialog, mod_menu, scroll + choice, mod_width, mod_height);

   }				/* auto-open module frame */
  while (keep_going)
   {
     wmove (dialog, 0, COLS - 1);
     wrefresh (dialog);
     key = mouse_wgetch (dialog);
     if (mod_menu_on)
      {

	if (button == BUTTON_OK)
	  switch (mod_nav
		  (dialog, mod_menu, scroll + choice,
		   (mod_width / 2) + box_x + x, box_y + y + choice + 1, key))
	   {
	   case 1:
	     continue;
	   case 2:
	     return 0;
	   }

      }

     if (button == BUTTON_OK)
      {
	/* Check if key pressed matches first character of any
	   item tag in menu */
/*	for (i = 0; i < max_choice; i++)
	  if (i != choice
	      && toupper (key) == toupper (groups[scroll + i].textstr[0]))
	    break;
*/

	if (!mod_menu_on)
	 {
	   int found = 0;

/* forward check */
	   for (i = scroll + choice + 1; i < groups.size (); i++)
	     if (toupper (key) == toupper (groups[i].textstr[0]))
	      {
		int newi = 0;

		while (scroll + choice != i)
		 {
		   print_sec_item (menu, groups[scroll + choice].textstr,
				   "", choice, FALSE);
		   /* auto-close module frame */
		   if (yast_mod_auto && modules[scroll + choice].size () > 0)
		    {
		      int max_items = MIN (max_mod_dsp,
					   modules[scroll + choice].size ());

		      mod_menu_on = 0;
		      yast_del_box (dialog, box_y + choice,
				    2 + box_x + menu_width,
				    max_items + 2, mod_width + 2,
				    dialog_attr, dialog_attr);
		      wnoutrefresh (menu);
		      wrefresh (dialog);
		    }		/* auto-close module frame */
		   if (scroll + max_choice < groups.size ())
		    {
		      scrollok (menu, TRUE);
		      wscrl (menu, 1);
		      scrollok (menu, FALSE);
		      scroll++;
		    }
		   else
		     choice++;
		 }		/* while */
		print_sec_item (menu, groups[scroll + choice].textstr,
				"", choice, TRUE);
		/* auto-open module frame */
		if (yast_mod_auto && modules[scroll + choice].size () > 0)
		 {
		   int max_items = MIN (INFO_Y - (box_y + choice) - 2,
					modules[scroll + choice].size ());
		   int i, max_len = 0, tmplen;

		   mod_height = max_mod_dsp = max_items;
		   for (i = 0; i < modules[scroll + choice].size (); i++)
		     if ((tmplen =
			  modules[scroll + choice][i].textstr.size ()) >
			 max_len)
		       mod_width = max_len =
			 yast_dyn_mod ? MIN (max_mod_width,
					     tmplen + 2) : max_mod_width;

		   print_sec_item (menu, groups[scroll + choice].textstr,
				   "->", choice, TRUE);
		   wnoutrefresh (menu);
		   mod_menu =
		     subwin (dialog, mod_height, mod_width,
			     box_y + choice + y + 1,
			     3 + x + box_x + menu_width);
		   yast_draw_box (dialog, box_y + choice,
				  2 + box_x + menu_width,
				  max_items + 2, mod_width + 2,
				  menubox_border_attr, inputbox_attr);
		   yast_mod_menu (dialog, mod_menu, scroll + choice,
				  mod_width, mod_height);

		 }		/* auto-open module frame */
		if (choice < max_choice)
		  for (newi = choice + 1; newi < max_choice; newi++)
		   {
		     print_sec_item (menu, groups[scroll + newi].textstr,
				     "", newi, FALSE);
		   }

		wnoutrefresh (menu);
		found = 1;
		break;
	      }

	   if (found)
	     continue;

	   /* backward check */
	   for (i = 0; i < scroll + choice; i++)
	     if (toupper (key) == toupper (groups[i].textstr[0]))
	      {
		int newi = 0;

		print_sec_item (menu, groups[scroll + choice].textstr,
				"", choice, FALSE);
		/* auto-close module frame */
		if (yast_mod_auto && modules[scroll + choice].size () > 0)
		 {
		   int max_items = MIN (max_mod_dsp,
					modules[scroll + choice].size ());

		   mod_menu_on = 0;
		   yast_del_box (dialog, box_y + choice,
				 2 + box_x + menu_width,
				 max_items + 2, mod_width + 2,
				 dialog_attr, dialog_attr);
		   wnoutrefresh (menu);
		   wrefresh (dialog);
		 }		/* auto-close module frame */
		scrollok (menu, TRUE);
		wscrl (menu, scroll * -1);
		scrollok (menu, FALSE);
		scroll = 0;
		choice = 0;
		while (scroll + choice != i)
		 {
		   print_sec_item (menu, groups[scroll + choice].textstr,
				   "", choice, FALSE);
		   /* auto-close module frame */
		   if (yast_mod_auto && modules[scroll + choice].size () > 0)
		    {
		      int max_items = MIN (max_mod_dsp,
					   modules[scroll + choice].size ());

		      mod_menu_on = 0;
		      yast_del_box (dialog, box_y + choice,
				    2 + box_x + menu_width,
				    max_items + 2, mod_width + 2,
				    dialog_attr, dialog_attr);
		      wnoutrefresh (menu);
		      wrefresh (dialog);
		    }		/* auto-close module frame */
		   if (scroll + max_choice < groups.size ())
		    {
		      scrollok (menu, TRUE);
		      wscrl (menu, 1);
		      scrollok (menu, FALSE);
		      scroll++;
		    }
		   else
		     choice++;

		 }		/* while */
		print_sec_item (menu, groups[scroll + choice].textstr,
				"", choice, TRUE);
		/* auto-open module frame */
		if (yast_mod_auto && modules[scroll + choice].size () > 0)
		 {
		   int max_items = MIN (INFO_Y - (box_y + choice) - 2,
					modules[scroll + choice].size ());
		   int i, max_len = 0, tmplen;

		   mod_height = max_mod_dsp = max_items;
		   for (i = 0; i < modules[scroll + choice].size (); i++)
		     if ((tmplen =
			  modules[scroll + choice][i].textstr.size ()) >
			 max_len)
		       mod_width = max_len =
			 yast_dyn_mod ? MIN (max_mod_width,
					     tmplen + 2) : max_mod_width;

		   print_sec_item (menu, groups[scroll + choice].textstr,
				   "->", choice, TRUE);
		   wnoutrefresh (menu);
		   mod_menu =
		     subwin (dialog, mod_height, mod_width,
			     box_y + choice + y + 1,
			     3 + x + box_x + menu_width);
		   yast_draw_box (dialog, box_y + choice,
				  2 + box_x + menu_width,
				  max_items + 2, mod_width + 2,
				  menubox_border_attr, inputbox_attr);
		   yast_mod_menu (dialog, mod_menu, scroll + choice,
				  mod_width, mod_height);

		 }		/* auto-open module frame */
		if (choice < max_choice)
		  for (newi = choice + 1; newi < max_choice; newi++)
		    print_sec_item (menu, groups[scroll + newi].textstr,
				    "", newi, FALSE);

		wnoutrefresh (menu);
		found = 1;
		break;
	      }

	   if (found)
	     continue;

	 }

	if ((key >= '1' && key <= MIN ('9', '0' + max_choice)) ||
	    key == KEY_UP || key == KEY_DOWN || key == '-' ||
	    key == '+' || (key >= M_EVENT && key - M_EVENT < ' '))
	 {
	   if (key >= '1' && key <= MIN ('9', '0' + max_choice))
	     i = key - '1';
	   else if (key >= M_EVENT)
	     i = key - M_EVENT;
	   else if (key == KEY_UP || key == '-')
	    {
	      if (!choice)
	       {
		 if (scroll)
		  {

		    /* Scroll menu down */
		    getyx (dialog, cur_y, cur_x);
		    if (menu_height > 1)
		     {
		       /* De-highlight current first item */
		       print_sec_item (menu, groups[scroll].textstr,
				       " ", 0, FALSE);
		       scrollok (menu, TRUE);
		       wscrl (menu, -1);
		       scrollok (menu, FALSE);

		       /* auto-close module frame */
		       if (yast_mod_auto
			   && modules[scroll + choice].size () > 0)
			{
			  int max_items = MIN (max_mod_dsp,
					       modules[scroll +
						       choice].size ());
			  mod_menu_on = 0;
			  yast_del_box (dialog, box_y + choice,
					2 + box_x + menu_width,
					max_items + 2, mod_width + 2,
					dialog_attr, dialog_attr);
			  wnoutrefresh (menu);
			  wrefresh (dialog);
			}	/* auto-close module frame */
		     }
		    scroll--;
		    print_sec_item (menu, groups[scroll].textstr,
				    " ", 0, TRUE);
		    /* auto-open module frame */
		    if (yast_mod_auto)
		     {
		       int max_items = MIN (INFO_Y - (box_y + choice) - 2,
					    modules[scroll + choice].size ());
		       int i, max_len = 0, tmplen;

		       mod_height = max_mod_dsp = max_items;
		       for (i = 0; i < modules[scroll + choice].size (); i++)
			 if ((tmplen =
			      modules[scroll + choice][i].textstr.size ()) >
			     max_len)
			   mod_width = max_len =
			     yast_dyn_mod ? MIN (max_mod_width,
						 tmplen + 2) : max_mod_width;

		       print_sec_item (menu, groups[scroll + choice].textstr,
				       "->", choice, TRUE);
		       wnoutrefresh (menu);
		       mod_menu =
			 subwin (dialog, mod_height, mod_width,
				 box_y + choice + y + 1,
				 3 + x + box_x + menu_width);
		       yast_draw_box (dialog, box_y + choice,
				      2 + box_x + menu_width,
				      max_items + 2, mod_width + 2,
				      menubox_border_attr, inputbox_attr);
		       yast_mod_menu (dialog, mod_menu, scroll + choice,
				      mod_width, mod_height);

		     }		/* auto-open module frame */
		    wnoutrefresh (menu);
		    wrefresh (dialog);
		    /* print the up/down arrows */
		    wmove (dialog, box_y, box_x + tag_x + 1);
		    wattrset (dialog, scroll ? uarrow_attr : menubox_attr);
		    waddch (dialog, scroll ? ACS_UARROW : ACS_HLINE);
		    wmove (dialog, box_y, box_x + tag_x + 2);
		    waddch (dialog, scroll ? '(' : ACS_HLINE);
		    wmove (dialog, box_y, box_x + tag_x + 3);
		    waddch (dialog, scroll ? '-' : ACS_HLINE);
		    wmove (dialog, box_y, box_x + tag_x + 4);
		    waddch (dialog, scroll ? ')' : ACS_HLINE);
		    wattrset (dialog, darrow_attr);
		    wmove (dialog, box_y + menu_height + 1,
			   box_x + tag_x + 1);
		    waddch (dialog, ACS_DARROW);
		    wmove (dialog, box_y + menu_height + 1,
			   box_x + tag_x + 2);
		    waddstr (dialog, "(+)");
		    wmove (dialog, cur_y, cur_x);
		    wrefresh (dialog);
		  }
		 continue;	/* wait for another key press */
	       }
	      else
		i = choice - 1;
	    }
	   else
	    {
	      if (key == KEY_DOWN || key == '+')
	       {
		 if (choice == max_choice - 1)
		  {
		    if (scroll + choice < item_no - 1)
		     {
		       /* Scroll menu up */
		       getyx (dialog, cur_y, cur_x);
		       if (menu_height > 1)
			{
			  /* De-highlight current last item */
			  print_sec_item (menu,
					  groups[scroll + max_choice -
						 1].textstr, " ",
					  max_choice - 1, FALSE);
			  scrollok (menu, TRUE);
			  scroll (menu);
			  scrollok (menu, FALSE);
			  /* auto-close module frame  */
			  if (yast_mod_auto
			      && modules[scroll + choice].size () > 0)
			   {
			     int max_items = MIN (max_mod_dsp,
						  modules[scroll +
							  choice].size ());
			     mod_menu_on = 0;
			     yast_del_box (dialog, box_y + choice,
					   2 + box_x + menu_width,
					   max_items + 2, mod_width + 2,
					   dialog_attr, dialog_attr);
			     wnoutrefresh (menu);
			     wrefresh (dialog);
			   }	/* auto-close module frame */
			}
		       scroll++;
		       print_sec_item (menu,
				       groups[scroll + max_choice -
					      1].textstr, " ", max_choice - 1,
				       TRUE);
		       wnoutrefresh (menu);

		       /* print the up/down arrows */
		       wattrset (dialog, uarrow_attr);
		       wmove (dialog, box_y, box_x + tag_x + 1);
		       waddch (dialog, ACS_UARROW);
		       wmove (dialog, box_y, box_x + tag_x + 2);
		       waddstr (dialog, "(-)");
		       wmove (dialog, box_y + menu_height + 1,
			      box_x + tag_x + 1);
		       wattrset (dialog,
				 scroll + choice <
				 item_no -
				 1 ? darrow_attr : menubox_border_attr);
		       waddch (dialog,
			       scroll + choice <
			       item_no - 1 ? ACS_DARROW : ACS_HLINE);
		       wmove (dialog, box_y + menu_height + 1,
			      box_x + tag_x + 2);
		       waddch (dialog,
			       scroll + choice <
			       item_no - 1 ? '(' : ACS_HLINE);
		       wmove (dialog, box_y + menu_height + 1,
			      box_x + tag_x + 3);
		       waddch (dialog,
			       scroll + choice <
			       item_no - 1 ? '+' : ACS_HLINE);
		       wmove (dialog, box_y + menu_height + 1,
			      box_x + tag_x + 4);
		       waddch (dialog,
			       scroll + choice <
			       item_no - 1 ? ')' : ACS_HLINE);
		       wmove (dialog, cur_y, cur_x);
		       wrefresh (dialog);
		     }
		    continue;	/* wait for another key press */
		  }
		 else
		   i = choice + 1;
	       }		/* if */
	    }			/* else */

	   if (i != choice)
	    {
	      /* De-highlight current item */
	      getyx (dialog, cur_y, cur_x);	/* Save cursor position */
	      print_sec_item (menu, groups[scroll + choice].textstr,
			      " ", choice, FALSE);
	      /* auto-close  module frame */
	      if (yast_mod_auto && modules[scroll + choice].size () > 0)
	       {
		 int max_items =
		   MIN (max_mod_dsp, modules[scroll + choice].size ());
		 mod_menu_on = 0;
		 yast_del_box (dialog, box_y + choice,
			       2 + box_x + menu_width, max_items + 2,
			       mod_width + 2, dialog_attr, dialog_attr);
		 wnoutrefresh (menu);
		 wrefresh (dialog);
	       }		/* auto-close module frame */

	      /* Highlight new item */
	      choice = i;
	      print_sec_item (menu, groups[scroll + choice].textstr,
			      " ", choice, TRUE);
	      wnoutrefresh (menu);
	      /* auto-open module frame */
	      if (yast_mod_auto && modules[scroll + choice].size () > 0)
	       {
		 int max_items = MIN (INFO_Y - (box_y + choice) - 2,
				      modules[scroll + choice].size ());
		 int i, max_len = 0, tmplen;

		 mod_height = max_mod_dsp = max_items;
		 for (i = 0; i < modules[scroll + choice].size (); i++)
		   if ((tmplen =
			modules[scroll + choice][i].textstr.size ()) >
		       max_len)
		     mod_width = max_len =
		       yast_dyn_mod ? MIN (max_mod_width,
					   tmplen + 2) : max_mod_width;

		 print_sec_item (menu, groups[scroll + choice].textstr,
				 "->", choice, TRUE);
		 wnoutrefresh (menu);
		 mod_menu =
		   subwin (dialog, mod_height, mod_width,
			   box_y + choice + y + 1,
			   3 + x + box_x + menu_width);
		 yast_draw_box (dialog, box_y + choice,
				2 + box_x + menu_width, max_items + 2,
				mod_width + 2, menubox_border_attr,
				inputbox_attr);
		 yast_mod_menu (dialog, mod_menu, scroll + choice, mod_width,
				mod_height);

	       }		/* auto-open module frame */
	      wmove (dialog, cur_y, cur_x);
	      wrefresh (dialog);
	    }
	   continue;		/* wait for another key press */
	 }

      }				/* button==BUTTON_OK */
     switch (key)
      {

      case CTRL_B:
	if (button > 0)
	  button--;
	else
	  button = BUTTONCNT;
	yast_print_button (dialog, button_help, button_y,
			   3, (button == BUTTON_HELP) ? TRUE : FALSE);
	yast_print_button (dialog, button_cancel, button_y,
			   (COLS - 5) - strlen (button_cancel),
			   (button == BUTTON_CANCEL) ? TRUE : FALSE);

	if (button != BUTTON_OK)
	 {
	   if (mod_menu_on)
	    {
	      yast_draw_frame (dialog, box_y + choice,
			       2 + box_x + menu_width, MIN (max_mod_dsp,
							    modules[scroll +
								    choice].
							    size ()) + 2,
			       mod_width + 2, menubox_border_attr,
			       inputbox_attr);
	      mod_nav (dialog, mod_menu, scroll + choice,
		       (mod_width / 2) + box_x + x, box_y + y + choice + 1,
		       DEHIGHLIGHT);
	    }
	   yast_draw_frame (dialog, box_y, box_x, menu_height + 2,
			    menu_width + 2, menubox_border_attr,
			    inputbox_attr);
	   print_sec_item (menu, groups[scroll + choice].textstr,
			   yast_mod_auto ? "->" : " ", choice, FALSE);
	 }
	else
	 {
	   if (mod_menu_on)
	    {
	      yast_draw_frame (dialog, box_y + choice,
			       2 + box_x + menu_width, MIN (max_mod_dsp,
							    modules[scroll +
								    choice].
							    size ()) + 2,
			       mod_width + 2, menubox_border_attr,
			       menubox_attr);
	      mod_nav (dialog, mod_menu, scroll + choice,
		       (mod_width / 2) + box_x + x, box_y + y + choice + 1,
		       HIGHLIGHT);
	    }
	   else
	     print_sec_item (menu, groups[scroll + choice].textstr,
			     yast_mod_auto ? "->" : " ", choice, TRUE);

	   yast_draw_frame (dialog, box_y, box_x, menu_height + 2,
			    menu_width + 2, menubox_border_attr,
			    menubox_attr);

	 }
	wrefresh (dialog);
	wrefresh (dialog);
	break;

      case ESC:		/* check for alt+tab */
       {
	 int altkey = 0;

	 nodelay (dialog, TRUE);
	 altkey = mouse_wgetch (dialog);
	 if (toupper (altkey) == toupper (button_cancel[0]))
	  {
	    nodelay (dialog, FALSE);
	    return 1;
	  }
	 if (toupper (altkey) == toupper (button_help[0]))
	  {
	    nodelay (dialog, FALSE);
	    return yast_help (dialog);
	  }
	 if (altkey == TAB)
	  {
	    nodelay (dialog, FALSE);

	    if (button > 0)
	      button--;
	    else
	      button = BUTTONCNT;
	    yast_print_button (dialog, button_help, button_y,
			       3, (button == BUTTON_HELP) ? TRUE : FALSE);
	    yast_print_button (dialog, button_cancel, button_y,
			       (COLS - 5) - strlen (button_cancel),
			       (button == BUTTON_CANCEL) ? TRUE : FALSE);
	    if (button != BUTTON_OK)
	     {
	       if (mod_menu_on)
		{
		  yast_draw_frame (dialog, box_y + choice,
				   2 + box_x + menu_width, MIN (max_mod_dsp,
								modules[scroll
									+
									choice].
								size ()) + 2,
				   mod_width + 2, menubox_border_attr,
				   inputbox_attr);
		  mod_nav (dialog, mod_menu, scroll + choice,
			   (mod_width / 2) + box_x + x,
			   box_y + y + choice + 1, DEHIGHLIGHT);
		}
	       yast_draw_frame (dialog, box_y, box_x, menu_height + 2,
				menu_width + 2, menubox_border_attr,
				inputbox_attr);
	       print_sec_item (menu, groups[scroll + choice].textstr,
			       yast_mod_auto ? "->" : " ", choice, FALSE);
	     }
	    else
	     {
	       if (mod_menu_on)
		{
		  yast_draw_frame (dialog, box_y + choice,
				   2 + box_x + menu_width, MIN (max_mod_dsp,
								modules[scroll
									+
									choice].
								size ()) + 2,
				   mod_width + 2, menubox_border_attr,
				   menubox_attr);
		  mod_nav (dialog, mod_menu, scroll + choice,
			   (mod_width / 2) + box_x + x,
			   box_y + y + choice + 1, HIGHLIGHT);
		}
	       else
		 print_sec_item (menu, groups[scroll + choice].textstr,
				 yast_mod_auto ? "->" : " ", choice, TRUE);

	       yast_draw_frame (dialog, box_y, box_x, menu_height + 2,
				menu_width + 2, menubox_border_attr,
				menubox_attr);
	     }
	    wrefresh (dialog);
	    break;
	  }			/* alt+tab */
	 else
	   nodelay (dialog, FALSE);
       }

      case KEY_LEFT:
	if (button != BUTTON_OK)
	  break;
	if (mod_menu_on)
	 {
	   int max_items = MIN (INFO_Y - (box_y + choice) - 2,
				modules[scroll + choice].size ());

	   mod_height = max_mod_dsp = max_items;
	   mod_menu_on = 0;
	   if (!yast_mod_auto)
	    {
	      print_sec_item (menu, groups[scroll + choice].textstr,
			      " ", choice, TRUE);
	      yast_del_box (dialog, box_y + choice, 2 + box_x + menu_width,
			    max_items + 2, mod_width + 2, dialog_attr,
			    dialog_attr);
	      print_mod_info (dialog, "");
	      wnoutrefresh (menu);
	      wrefresh (dialog);
	    }
	   else
	    {
	      print_mod_info (dialog, "");
	      yast_draw_frame (dialog, box_y + choice, 2 + box_x + menu_width,
			       max_items + 2, mod_width + 2,
			       menubox_border_attr, inputbox_attr);
	      mod_nav (dialog, mod_menu, scroll + choice,
		       (mod_width / 2) + box_x + x, box_y + y + choice + 1,
		       NAV_RESET);
	      print_sec_item (menu, groups[scroll + choice].textstr,
			      "->", choice, TRUE);
	      wrefresh (dialog);
	    }
	 }
	break;

/* buttoncancel */
      case '\n':
	if (button == BUTTON_CANCEL
	    || (!groups[scroll + choice].textstr.compare (button_cancel)
		&& button == BUTTON_OK))
	 {
	   keep_going = 0;
	   break;
	 }

	if (button == BUTTON_HELP)
	 {
	   yast_help (dialog);
	   return 0;
	   break;
	 }
      case KEY_RIGHT:
	if (button != BUTTON_OK)
	  break;
	if (!mod_menu_on && modules[scroll + choice].size () > 0)
	 {
	   int max_items = MIN (INFO_Y - (box_y + choice) - 2,
				modules[scroll + choice].size ());
	   int i, max_len = 0, tmplen;

	   max_mod_dsp = max_items;
	   mod_height = max_mod_dsp;
	   mod_menu_on = 1;
	   for (i = 0; i < modules[scroll + choice].size (); i++)
	     if ((tmplen =
		  modules[scroll + choice][i].textstr.size ()) > max_len)
	       mod_width = max_len =
		 yast_dyn_mod ? MIN (max_mod_width,
				     tmplen + 2) : max_mod_width;

	   print_sec_item (menu, groups[scroll + choice].textstr,
			   "->", choice, FALSE);
	   if (!yast_mod_auto)
	    {
	      mod_menu =
		subwin (dialog, mod_height, mod_width, box_y + choice + y + 1,
			3 + x + box_x + menu_width);
	      yast_draw_box (dialog, box_y + choice,
			     2 + box_x + menu_width, max_items + 2,
			     mod_width + 2, menubox_border_attr,
			     menubox_attr);
	      yast_mod_menu (dialog, mod_menu, scroll + choice, mod_width,
			     mod_height);
	    }
	   else
	     yast_draw_frame (dialog, box_y + choice, 2 + box_x + menu_width,
			      max_items + 2, mod_width + 2,
			      menubox_border_attr, menubox_attr);

	   /*  init the menu-navigation */
	   mod_nav (dialog, mod_menu, scroll + choice,
		    (mod_width / 2) + box_x + x, box_y + y + choice + 1,
		    (oldchoice==choice)?HIGHLIGHT:NAV_INIT);
oldchoice=choice;
	 }
	wnoutrefresh (menu);
	wrefresh (dialog);
	break;

      case TAB:
      case CTRL_F:
	if (button < BUTTONCNT)
	  button++;
	else
	  button = 0;
	yast_print_button (dialog, button_help, button_y,
			   3, (button == BUTTON_HELP) ? TRUE : FALSE);
	yast_print_button (dialog, button_cancel, button_y,
			   (COLS - 5) - strlen (button_cancel),
			   (button == BUTTON_CANCEL) ? TRUE : FALSE);
	if (button != BUTTON_OK)
	 {
	   if (mod_menu_on)
	    {
	      yast_draw_frame (dialog, box_y + choice, 2 + box_x + menu_width,
			       MIN (max_mod_dsp,
				    modules[scroll + choice].size ()) + 2,
			       mod_width + 2, menubox_border_attr,
			       inputbox_attr);
	      mod_nav (dialog, mod_menu, scroll + choice,
		       (mod_width / 2) + box_x + x, box_y + y + choice + 1,
		       DEHIGHLIGHT);
	    }
	   yast_draw_frame (dialog, box_y, box_x, menu_height + 2,
			    menu_width + 2, menubox_border_attr,
			    inputbox_attr);
	   print_sec_item (menu, groups[scroll + choice].textstr,
			   yast_mod_auto ? "->" : " ", choice, FALSE);
	 }
	else
	 {
	   if (mod_menu_on)
	    {
	      yast_draw_frame (dialog, box_y + choice, 2 + box_x + menu_width,
			       MIN (max_mod_dsp,
				    modules[scroll + choice].size ()) + 2,
			       mod_width + 2, menubox_border_attr,
			       menubox_attr);
	      mod_nav (dialog, mod_menu, scroll + choice,
		       (mod_width / 2) + box_x + x, box_y + y + choice + 1,
		       HIGHLIGHT);
	    }
	   else
	     print_sec_item (menu, groups[scroll + choice].textstr,
			     yast_mod_auto ? "->" : " ", choice, TRUE);

	   yast_draw_frame (dialog, box_y, box_x, menu_height + 2,
			    menu_width + 2, menubox_border_attr,
			    menubox_attr);
	 }
	wrefresh (dialog);
	break;
      }
   }

  delwin (dialog);
  return -1;			/* ESC pressed */
}

void
yast_grp_menu (WINDOW * dialog, WINDOW * menu, int choice, int width,
	       int height)
{
  int i, item_no = groups.size ();
  int max_choice = MIN (MAX_GRP_DSP, groups.size ());

  tag_x = 0;
  item_x = 0;
  for (i = 0; i < item_no; i++)
   {
     tag_x = MAX (tag_x, groups[i].textstr.size () + 2);
     item_x = MAX (item_x, groups[i].textstr.size ());
   }

/*    tag_x = (menu_width - tag_x) / 2; */
  tag_x = 1;
  item_x = tag_x + item_x + 2;

  /* Print the menu */

  for (i = 0; i < max_choice; i++)
    print_sec_item (menu, groups[i].textstr, "", i, i == choice);
  wnoutrefresh (menu);
  wrefresh (dialog);

  if (height < item_no)
   {
     wattrset (dialog, darrow_attr);
     wmove (dialog, box_y + height + 1, box_x + tag_x + 1);
     waddch (dialog, ACS_DARROW);
     wmove (dialog, box_y + height + 1, box_x + tag_x + 2);
     waddstr (dialog, "(+)");
   }
}

int yast_help (WINDOW * main)
{
  int winhight = LINES - 10, winwidth = COLS - 6;
  int i = 0;

  WINDOW *helpwin = subwin (main, winhight, winwidth, 5, 3);

  yast_draw_box (main, 4, 2, LINES - 5, COLS - 4, menubox_border_attr,
		 menubox_attr);
  yast_print_button (main, "Ok", LINES - 4, ((COLS - 4) / 2), TRUE);

  for (i = 0; i < winhight && i < helptext.size (); i++)
    print_help_item (helpwin, helptext[i], "", i, FALSE, winwidth);

  while (1)
   {
     wrefresh (main);
     int key = mouse_wgetch (main);

     switch (key)
      {
      case KEY_DOWN:
	if (i < helptext.size ())
	 {
	   scrollok (helpwin, TRUE);
	   wscrl (helpwin, 1);
	   scrollok (helpwin, FALSE);
	   print_help_item (helpwin, helptext[i], "", winhight - 1, FALSE,
			    winwidth);
	   i++;
	 }
	break;
      case KEY_UP:
	if (i > winhight)
	 {
	   scrollok (helpwin, TRUE);
	   wscrl (helpwin, -1);
	   scrollok (helpwin, FALSE);
	   i--;
	   print_help_item (helpwin, helptext[i - winhight], "", 0, FALSE,
			    winwidth);
	 }
	break;
      case '\n':
	return 0;
      }
   }				/* while */
}
