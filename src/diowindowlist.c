/* diowindowlist
 * this shows a button, when you click on it, you get the list of all opened windows
 */

#include <glib.h>
#include <ctype.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "configsgen.c"
#include "externvars.h"
#include "outputtochar.h"
#include "replacenthline.h"
#include "getvaluefromconf.h"
#include "outputtochararray.h"
#include "getfilenamecontainingsubstr.h"

#define APPNAME popWindowList

GtkApplication	*app;
GtkWidget		*windowR;
GtkWidget		*APPNAME;
GtkStringList	*myStringsIcons;
GtkStringList	*myStringsNames;
GtkAdjustment	*adjustmentPosx;
GtkAdjustment	*adjustmentPosy;
GtkWidget		*popWindowListMenu;
GtkAdjustment	*adjustmentSizeWidth;
GtkAdjustment	*adjustmentSizeHeight;

static void remove_button() {
	for (int i = 0; *windowNames[i] != NULL; i++) {
		for (int j = 0; j < 1; j++) {
			*icons[i] = NULL;
			free((void *)icons[i][j]);
			*windowNames[i] = NULL;
			free((void *)windowNames[i][j]);
		}
	}

	gtk_popover_popdown(GTK_POPOVER(APPNAME));
	gtk_window_close(GTK_WINDOW(windowR));
	g_application_quit(G_APPLICATION(app));
}

static void close_save_cb(GtkWidget *window) {
	gint width = gtk_adjustment_get_value(adjustmentSizeWidth);
	gchar widthText[30];
	sprintf(widthText, "width=%d", width);
	replacenthline(pathToConfig, 1, (const char *)widthText);

	gint height = gtk_adjustment_get_value(adjustmentSizeHeight);
	gchar heightText[30];
	sprintf(heightText, "height=%d", height);
	replacenthline(pathToConfig, 2, (const char *)heightText);

	gint posx = gtk_adjustment_get_value(adjustmentPosx);
	gchar posxText[30];
	sprintf(posxText, "posx=%d", posx);
	replacenthline(pathToConfig, 3, (const char *)posxText);

	gint posy = gtk_adjustment_get_value(adjustmentPosy);
	gchar posyText[30];
	sprintf(posyText, "posy=%d", posy);
	replacenthline(pathToConfig, 4, (const char *)posyText);

	gtk_widget_set_size_request(APPNAME, width, height);
	gtk_popover_set_offset(GTK_POPOVER(APPNAME), posx, posy);
	gtk_popover_popdown(GTK_POPOVER(APPNAME));
	gtk_popover_popup(GTK_POPOVER(APPNAME));
	gtk_window_close(GTK_WINDOW(window));
}

static void on_right_click() {
	gint width				= get_int_value_from_conf(pathToConfig, "width");
	gint height				= get_int_value_from_conf(pathToConfig, "height");
	gint posx				= get_int_value_from_conf(pathToConfig, "posx");
	gint posy				= get_int_value_from_conf(pathToConfig, "posy");

	windowR = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(windowR), "DioAppHider Settings");
	gtk_window_set_icon_name(GTK_WINDOW(windowR), "application-x-deb");

	GtkWidget *removeButton;
	removeButton = gtk_button_new_with_label("Remove this panel");
	gtk_widget_set_halign(removeButton, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_start(removeButton, 10);
	gtk_widget_set_margin_end(removeButton, 10);

	GtkWidget *closeButton;
	closeButton = gtk_button_new_with_label("Close/Apply");
	gtk_widget_set_halign(closeButton, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_start(closeButton, 10);
	gtk_widget_set_margin_end(closeButton, 10);

	// panel width
	GtkWidget *labelWidth;
	labelWidth = gtk_label_new("Panel Width");

	adjustmentSizeWidth = gtk_adjustment_new(width, 0, 700, 10, 1, 1);

	GtkWidget *entrySizeWidth;
	entrySizeWidth = gtk_spin_button_new(adjustmentSizeWidth, 1, 0);

	// panel height
	GtkWidget *labelHeight;
	labelHeight = gtk_label_new("Panel Height");

	adjustmentSizeHeight = gtk_adjustment_new(height, 0, 700, 10, 1, 1);

	GtkWidget *entrySizeHeight;
	entrySizeHeight = gtk_spin_button_new(adjustmentSizeHeight, 1, 0);

	GtkWidget *boxWidthHeight;
	boxWidthHeight = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(boxWidthHeight), TRUE);
	gtk_box_prepend(GTK_BOX(boxWidthHeight), labelWidth);
	gtk_box_append(GTK_BOX(boxWidthHeight), entrySizeWidth);
	gtk_box_append(GTK_BOX(boxWidthHeight), labelHeight);
	gtk_box_append(GTK_BOX(boxWidthHeight), entrySizeHeight);

	// panel position x
	GtkWidget *labelX;
	labelX = gtk_label_new("Position X");

	adjustmentPosx = gtk_adjustment_new(posx, 0, 4000, 10, 1, 1);

	GtkWidget *entryPosx;
	entryPosx = gtk_spin_button_new(adjustmentPosx, 1, 0);

	// panel position y
	GtkWidget *labelY;
	labelY = gtk_label_new("Position Y");

	adjustmentPosy = gtk_adjustment_new(posy, 0, 4000, 10, 1, 1);

	GtkWidget *entryPosy;
	entryPosy = gtk_spin_button_new(adjustmentPosy, 1, 0);

	GtkWidget *boxPosition;
	boxPosition = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(boxPosition), TRUE);
	gtk_box_prepend(GTK_BOX(boxPosition), labelX);
	gtk_box_append(GTK_BOX(boxPosition), entryPosx);
	gtk_box_append(GTK_BOX(boxPosition), labelY);
	gtk_box_append(GTK_BOX(boxPosition), entryPosy);

	// main box that contains both vertical boxes
	GtkWidget *boxAll;
	boxAll = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_box_prepend(GTK_BOX(boxAll), boxWidthHeight);
	gtk_box_set_homogeneous(GTK_BOX(boxAll), TRUE);
	gtk_box_append(GTK_BOX(boxAll), boxPosition);
	gtk_box_append(GTK_BOX(boxAll), removeButton);
	gtk_box_append(GTK_BOX(boxAll), closeButton);

	g_signal_connect_swapped(removeButton, "clicked", G_CALLBACK(remove_button), windowR);
	g_signal_connect_swapped(closeButton, "clicked", G_CALLBACK(close_save_cb), windowR);

	gtk_window_set_child(GTK_WINDOW(windowR), boxAll);
	gtk_window_present(GTK_WINDOW(windowR));
}

/// on window list autohide
static void menu_closed(GtkWindow *window) {
	gtk_window_close(GTK_WINDOW(window));
}

/// creating labels to fill up the listview
static void setup_listitem(GtkListItemFactory *factory, GtkListItem *list_item) {
	(void)factory; // Marking the parameter as unused

	GtkWidget *label;
	label = gtk_label_new(NULL);

	// defining the size of text in label
	PangoAttrList *attr_list = pango_attr_list_new();
	PangoAttribute *attr = pango_attr_size_new_absolute(22 * PANGO_SCALE);
	pango_attr_list_insert(attr_list, attr);
	PangoLayout *layout = gtk_widget_create_pango_layout(label, NULL);
	pango_layout_set_attributes(layout, attr_list);

	gtk_label_set_attributes(GTK_LABEL(label), attr_list);

	GtkWidget *icon;
	icon = gtk_image_new();
	gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_LARGE);

	GtkWidget *box;
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
	gtk_box_prepend(GTK_BOX(box), icon);
	gtk_box_append(GTK_BOX(box), label);

	gtk_list_item_set_child(list_item, box);

	// freing resources
	g_object_unref(layout);
	pango_attr_list_unref(attr_list);
}

/// binding the items to the list, this function runs in a loop
static void bind_listitem(GtkListItemFactory *factory, GtkListItem *list_item) {
	(void)factory; // Marking the parameter as unused

	// Get the GtkBox widget
	GtkWidget *box;
	box = gtk_list_item_get_child(list_item);

	// Find the GtkImage widget within the GtkBox
	GtkWidget *icon;
	icon = gtk_widget_get_first_child(box);

	// Find the next child after GtkImage whoch is a label 
	GtkWidget *label;
	label = gtk_widget_get_next_sibling(icon);

	// gets the strings from GtkStringList *myStrings in startup function
	guint listitemPos = gtk_list_item_get_position(list_item); // index of items

	// getttins names to populate the menu
	const gchar *stringsNames = gtk_string_list_get_string(myStringsNames, listitemPos);

	// sets label text to strings one at a time
	gtk_label_set_label(GTK_LABEL(label), stringsNames);

	// get icons
	const gchar *stringsIcons = gtk_string_list_get_string(myStringsIcons, listitemPos);

	if (strcmp(stringsIcons, "empty") == 0) {
		gtk_image_set_from_icon_name(GTK_IMAGE(icon), "empty");
	}
	else {
		gtk_image_set_from_file(GTK_IMAGE(icon), (const gchar *)stringsIcons);
	}
}

/// runs the app when clicked on it
static void activate_item(GtkSingleSelection *mySingleSelection) {
	// gets currenty selected item
	GtkStringObject *myItem;
	myItem = gtk_single_selection_get_selected_item(GTK_SINGLE_SELECTION(mySingleSelection));

	const gchar *selectedItem = gtk_string_object_get_string(myItem);
	selectedItem = g_strstrip((gchar *)selectedItem);

	gtk_popover_popdown(GTK_POPOVER(popWindowListMenu));

	int pid = fork();

	if (pid == -1) {
		printf("Failed to fork\n");
		return;
	}

	if (pid == 0) {
		execlp("wmctrl", "wmctrl", "-a", selectedItem, NULL);
	}
	else {
		wait(NULL);
	}
}

/// action on left click
static void on_left_click() {
	char *bufferIcons[77];
	char *bufferNames[77];

	// Get the GSettings schema for org.gnome.desktop.interface
	GSettings *settings = g_settings_new("org.gnome.desktop.interface");
	// Get the current icon theme
	const gchar *iconTheme = g_settings_get_string(settings, "icon-theme");

	gchar themePath[1024];

	gchar *themePathSystem = g_strdup_printf("/usr/share/icons/%s", iconTheme);
	gchar *themePathLocal = g_strdup_printf("%s/.icons/%s", getenv("HOME"), iconTheme);

	// check if icon path exists
	FILE *iconPathCheckSystem = fopen(themePathSystem, "r");

	// if theme is not in /usr/share then look in .icons
	if (iconPathCheckSystem == NULL) {
		perror(themePathSystem);

		// set default icon path to local path
		snprintf(themePath, strlen(themePathLocal) + 1, "%s", themePathLocal);

		g_free(themePathSystem);
		themePathSystem = NULL;
	}
	else {
		// set default icon path to system path
		snprintf(themePath, strlen(themePathSystem) + 1, "%s", themePathSystem);
		g_free(themePathLocal);
		themePathLocal = NULL;
		fclose(iconPathCheckSystem);
	}

	if (strlen(themePath) == 0) {
		perror(themePathLocal);
		perror(themePathSystem);
		themePathLocal = NULL;
		themePathSystem = NULL;
		g_free(themePathLocal);
		g_free(themePathSystem);
	}

	g_print("Icon path: %s\n", themePath);

	const gchar *strStaticNrOfLines = output_to_char("wmctrl -lx | \
											awk '{print $3}' | awk -F '.' '{print $1}' | wc -l");

	// populating icons
	const gchar *commandI = "wmctrl -lx | awk '{print $3}' | awk -F '.' '{print $1}'";

	output_to_char_array(bufferIcons, commandI);

	gint iconsIdex;
	gint staticNrOfLines = atoi(strStaticNrOfLines);

	for (iconsIdex = 0; iconsIdex < staticNrOfLines; iconsIdex++) {
		*icons[iconsIdex] = get_filename_containing_substring(themePath, bufferIcons[iconsIdex]);
		if (*icons[iconsIdex] == NULL) {
			g_print("Not found icon: %s\n", bufferIcons[iconsIdex]);
			*icons[iconsIdex] = "empty";
		};
		bufferIcons[iconsIdex] = NULL;
		free(bufferIcons[iconsIdex]);
	}
	// null terminate icons string array
	*icons[iconsIdex] = NULL;

	// populating the names
	//const gchar *command = "wmctrl -lx | awk '{print $5\" \"$6}'";
	const gchar *command = "wmctrl -lx | \
								awk '{for (i = 5; i <= NF; i++) printf \"%s \", $i; print \"\"}'";

	output_to_char_array(bufferNames, command);

	gint i;

	for (i = 0; i < staticNrOfLines; i++) {
		*windowNames[i] = bufferNames[i];
		bufferNames[i] = NULL;
		free(bufferNames[i]);
	}
	*windowNames[i] = NULL;
		
	command = NULL;
	commandI = NULL;

	free((void *)command);
	free((void *)commandI);

	GtkWidget *window;
	window = gtk_window_new();

	gint xOffset;
	gint yOffset;

	gtk_popover_get_offset(GTK_POPOVER(APPNAME), &xOffset, &yOffset);

	// turns all strings containing names into objects
	myStringsIcons = gtk_string_list_new((const gchar * const *)icons);

	// turns all strings containing names into objects
	myStringsNames = gtk_string_list_new((const gchar * const *)windowNames);

	// creates a list model from the object of string objects
	GListModel *myGlistModel;
	myGlistModel = G_LIST_MODEL(myStringsNames);

	// creats a list item factory to setup and bind the string object to the list view
	GtkListItemFactory *myGtkListItemFactory;
	myGtkListItemFactory = gtk_signal_list_item_factory_new();

	// callbacks (funtions) to run on signals that widgets emit
	g_signal_connect(myGtkListItemFactory, "setup", G_CALLBACK(setup_listitem), NULL);
	g_signal_connect(myGtkListItemFactory, "bind", G_CALLBACK(bind_listitem), NULL);

	// defining how selection of items behaves
	GtkSingleSelection *mySingleSelection;
	mySingleSelection = gtk_single_selection_new(G_LIST_MODEL(myGlistModel));
	gtk_single_selection_set_autoselect(mySingleSelection, FALSE);
	gtk_single_selection_set_can_unselect(mySingleSelection, TRUE);

	GtkSelectionModel *mySelectionModel;
	mySelectionModel = GTK_SELECTION_MODEL(mySingleSelection);
	gtk_selection_model_unselect_all(mySelectionModel);
	gtk_selection_model_unselect_item(mySelectionModel, 0);

	// creating the list view to display the list items but it needs to be a child of a widget
	GtkWidget *myGtkListView;
	myGtkListView = gtk_list_view_new(mySelectionModel, myGtkListItemFactory);
	gtk_list_view_set_single_click_activate(GTK_LIST_VIEW(myGtkListView), TRUE);

	g_signal_connect_swapped(myGtkListView, "activate", G_CALLBACK(activate_item), mySingleSelection);

	// adding the list view as the child
	GtkWidget *listViewScrolled;
	listViewScrolled = gtk_scrolled_window_new();
	gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(listViewScrolled), TRUE);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(listViewScrolled), myGtkListView);
	gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(listViewScrolled), 500);

	gint listHeight = staticNrOfLines;
	if (listHeight > 20) { listHeight = 20; };

	GtkWidget *listviewFrame;
	listviewFrame = gtk_frame_new(NULL);
	gtk_widget_set_size_request(listviewFrame, 500, listHeight * 38);
	gtk_frame_set_child(GTK_FRAME(listviewFrame), listViewScrolled);

	GtkWidget *boxAll;
	boxAll = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
	gtk_box_prepend(GTK_BOX(boxAll), listviewFrame);

	popWindowListMenu = gtk_popover_new();
	gtk_popover_set_child(GTK_POPOVER(popWindowListMenu), boxAll);
	gtk_popover_set_has_arrow(GTK_POPOVER(popWindowListMenu), FALSE);
	gtk_popover_set_offset(GTK_POPOVER(popWindowListMenu), xOffset - 270, yOffset + 10);

	g_signal_connect_swapped(popWindowListMenu, "closed", G_CALLBACK(menu_closed), window);

	// freeing memory
	g_object_unref(settings);
	g_free(themePathSystem);
	g_free(themePathLocal);

	gtk_window_set_child(GTK_WINDOW(window), popWindowListMenu);
	gtk_popover_popup(GTK_POPOVER(popWindowListMenu));
}

/// activate signal handle
static void activate() {
	if (appstate == 1) {
		g_print("already running, only one instance allowed\n");
		return;
	}
	else {
		g_print("Welcome to DioWindowList!\n");
	}
	// sets flag that app is already running
	appstate = 1;
}

/// startup signal handle
static void startup(GtkApplication *app) {
	GtkWidget *window;
	window = gtk_application_window_new(GTK_APPLICATION(app));

	// get width and height from config
	int width = get_int_value_from_conf((char *)pathToConfig, "width");
	int height = get_int_value_from_conf((char *)pathToConfig, "height");
	int posx = get_int_value_from_conf((char *)pathToConfig, "posx");
	int posy = get_int_value_from_conf((char *)pathToConfig, "posy");

	APPNAME = gtk_popover_new();
	gtk_widget_set_size_request(APPNAME, width, height);
	gtk_popover_set_autohide(GTK_POPOVER(APPNAME), FALSE);
	gtk_popover_set_has_arrow(GTK_POPOVER(APPNAME), FALSE);
	gtk_popover_set_offset(GTK_POPOVER(APPNAME), posx, posy);

	// adds right click action for new panel
	GtkGesture *rclick;
	rclick = gtk_gesture_click_new();

	GtkGestureSingle *rsingleclick;
	rsingleclick = GTK_GESTURE_SINGLE(rclick);

	gtk_gesture_single_set_button(rsingleclick, 3);

	GtkEventController *rightclick;
	rightclick = GTK_EVENT_CONTROLLER(rclick);
	gtk_event_controller_set_propagation_phase(rightclick, GTK_PHASE_CAPTURE);
	gtk_widget_add_controller(APPNAME, rightclick);

	g_signal_connect_swapped(rightclick, "pressed", G_CALLBACK(on_right_click), NULL);

	// adds left click action for new panel
	GtkGesture *lclick;
	lclick = gtk_gesture_click_new();

	GtkGestureSingle *lsingleclick;
	lsingleclick = GTK_GESTURE_SINGLE(lclick);

	gtk_gesture_single_set_button(lsingleclick, 1);

	GtkEventController *leftClick;
	leftClick = GTK_EVENT_CONTROLLER(lclick);
	gtk_event_controller_set_propagation_phase(leftClick, GTK_PHASE_CAPTURE);
	gtk_widget_add_controller(APPNAME, leftClick);

	g_signal_connect_swapped(leftClick, "pressed", G_CALLBACK(on_left_click), NULL);

	gtk_window_set_child(GTK_WINDOW(window), APPNAME);
	gtk_popover_popup(GTK_POPOVER(APPNAME));
}

int main() {
	// deifning HOME
	HOME = getenv("HOME");

	// create initial config
	create_configs();

	// setting config file
	snprintf((char *)pathToConfig, 777, "%s%s", HOME, configName);

	gint status;
	app = gtk_application_new("com.github.DiogenesN.diowindowlist", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	g_signal_connect(app, "startup", G_CALLBACK(startup), NULL);
	status = g_application_run(G_APPLICATION(app), 0, NULL);
	g_object_unref(app);

	return status;
}
