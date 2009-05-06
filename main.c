/*
    Based on the gtk-launcher from webkit/gtk as seen in the license block below.
    License for the modified code is MIT License see LICENSE file.
*/
/*
 * Copyright (C) 2006, 2007 Apple Inc.
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <gdk/gdkkeysyms.h>

#if ! ((GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 16) || GTK_MAJOR_VERSION > 2)
#define GTK_ENTRY_ICON_PRIMARY 0
#define NO_FANCY_FUNCTIONS
#endif

#define JS_SETUP "function clearfocus() {\
 if(document.activeElement && document.activeElement.blur) \
        document.activeElement.blur();\
} \
function v(e, y) { \
t = e.nodeName.toLowerCase(); \
if((t == 'input' && /^(text|password)$/.test(e.type)) || /^(select|textarea)$/.test(t) || e.contentEditable == 'true') \
    console.log('insertmode_'+(y=='focus'?'on':'off')); \
} \
if(document.activeElement) \
    v(document.activeElement,'focus'); \
m=['focus','blur']; \
for(i in m) \
    document.getElementsByTagName('body')[0].addEventListener(m[i], function(x) { \
        v(x.target,x.type); \
    }, true); \
document.getElementsByTagName(\"body\")[0].appendChild(document.createElement(\"style\")); \
document.styleSheets[0].addRule('.hinting_mode_hint', 'color: #000; background: #ff0');\
document.styleSheets[0].addRule('.hinting_mode_hint_focus', 'color: #000; background: #8f0');\
function show_hints() { \
    var height = window.innerHeight; \
    var width = window.innerWidth; \
    var scrollX = document.defaultView.scrollX; \
    var scrollY = document.defaultView.scrollY; \
    var hinttags = \"//html:*[@onclick or @onmouseover or @onmousedown or @onmouseup or @oncommand or @class='lk' or @role='link'] | //html:input[not(@type='hidden')] | //html:a | //html:area | //html:iframe | //html:textarea | //html:button | //html:select\"; \
    var r = document.evaluate(hinttags, document, function(p) { return 'http://www.w3.org/1999/xhtml'; }, XPathResult.ORDERED_NODE_ITERATOR_TYPE, null); \
    div = document.createElement(\"div\"); \
    i = 1; \
    a = []; \
    while(elem = r.iterateNext()) \
    { \
        rect = elem.getBoundingClientRect(); \
        if (!rect || rect.top > height || rect.bottom < 0 || rect.left > width || rect.right < 0 || !(elem.getClientRects()[0])) \
            continue; \
        var computedStyle = document.defaultView.getComputedStyle(elem, null); \
        if (computedStyle.getPropertyValue(\"visibility\") != \"visible\" || computedStyle.getPropertyValue(\"display\") == \"none\") \
            continue; \
        var leftpos = Math.max((rect.left + scrollX), scrollX); \
        var toppos = Math.max((rect.top + scrollY), scrollY); \
        a.push(elem); \
        div.innerHTML += '<span id=\"hint' + i + '\" style=\"position: absolute; top: ' + toppos + 'px; left: ' + leftpos + 'px; background: red; color: #fff; font: bold 10px monospace; z-index: 99\">' + (i++) + '</span>';\
    } \
    for(e in a) \
        a[e].className += \" hinting_mode_hint\"; \
    document.getElementsByTagName(\"body\")[0].appendChild(div); \
    clearfocus(); \
    s = \"\"; \
    h = null; \
    window.onkeyup = function(e) \
    { \
        if(e.which == 13 && s != \"\") { \
            if(h) h.className = h.className.replace(\"_focus\",\"\"); \
            fire(parseInt(s)); \
        } \
        key = String.fromCharCode(e.which); \
        if (isNaN(parseInt(key))) \
            return; \
        s += key; \
        n = parseInt(s); \
        if(h != null) \
            h.className = h.className.replace(\"_focus\",\"\"); \
        if (i - 1 < n * 10) \
            fire(n); \
        else \
            (h = a[n - 1]).className = a[n - 1].className.replace(\"hinting_mode_hint\", \"hinting_mode_hint_focus\"); \
    }; \
} \
    function fire(n) \
    { \
        el = a[n - 1]; \
        tag = el.nodeName.toLowerCase(); \
        clear(); \
        if(tag == \"iframe\" || tag == \"frame\" || tag == \"textarea\" || (tag == \"input\" && el.type == \"text\")) \
            el.focus(); \
        else { \
            var evObj = document.createEvent('MouseEvents'); \
            evObj.initMouseEvent('click', true, true, window, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, null); \
            el.dispatchEvent(evObj); \
        } \
    } \
    function cleanup() \
    { \
        for(e in a) \
            a[e].className = a[e].className.replace(/hinting_mode_hint/,''); \
        div.parentNode.removeChild(div); \
        window.onkeyup = null; \
    } \
    function clear() \
    { \
        cleanup(); \
        console.log(\"hintmode_off\") \
    } "
#define JS_ENABLE_HINTS "show_hints();"

#define APPNAME   "WebKit Browser"

#define STARTPAGE "https://projects.ring0.de/webkitbrowser/"
#define SETTING_USER_CSS_FILE NULL
#define DOWNLOADER "xterm -e screen wget"

enum { MODE_NORMAL, MODE_INSERT, MODE_SEARCH, MODE_HINTS };
enum { TARGET_CURRENT, TARGET_NEW };

static GtkWidget* vbox;
static GtkWidget* main_window;
static GtkWidget* uri_entry;
static WebKitWebView* web_view;
static gchar* main_title;
static gint load_progress;
static int mode;  // vimperator mode
static int count; // cmdcounter for multiple command execution (vimperator style)
static gchar modkey; // vimperator modkey e.g. "g" für gg or gf
static int target;
static gfloat zoomstep;
static GtkClipboard* xclipboard;
static GtkClipboard* clipboard;
static char* cmd;

void exec(const char *param);

static void
activate_uri_entry_cb (GtkWidget* entry, gpointer data)
{
    guint c;
    const gchar* real_uri = NULL;
    const gchar* uri = gtk_entry_get_text((GtkEntry*)entry);
    g_assert(uri);
    if(mode == MODE_NORMAL) {
        if(target == TARGET_NEW)
            real_uri = webkit_web_view_get_uri(web_view);
        webkit_web_view_load_uri(web_view, uri);
        if(real_uri)
            gtk_entry_set_text((GtkEntry*)entry, real_uri);
    } else if(mode == MODE_SEARCH) {
        /* unmark the old hits if any */
        webkit_web_view_unmark_text_matches(web_view);
        /* the new search */
        c = webkit_web_view_mark_text_matches(web_view, uri, (gboolean)FALSE, 0);
        webkit_web_view_set_highlight_text_matches(web_view, (gboolean)TRUE);
        webkit_web_view_search_text(web_view, uri, (gboolean)FALSE, (gboolean)TRUE, (gboolean)TRUE);
    }
    /* focus webview */
    gtk_widget_grab_focus((GtkWidget*)web_view);
}

static void
find_next (gboolean direction)
{
    const gchar* txt = gtk_entry_get_text((GtkEntry*)uri_entry);
    webkit_web_view_search_text(web_view, txt, (gboolean)FALSE, (gboolean)direction, (gboolean)TRUE);
}

static void
zoom (int times, gboolean direction, gboolean fullzoom)
{
    webkit_web_view_set_full_content_zoom(web_view, fullzoom);
    webkit_web_view_set_zoom_level(web_view, webkit_web_view_get_zoom_level(web_view) + (gfloat)((direction ? 1.0 : -1.0) * zoomstep * (float)(times ? times : 1)));
}

static void
update_title (GtkWindow* window)
{
    GString* string = g_string_new(main_title);
    g_string_append(string, " - " APPNAME);
    if(load_progress < 100)
        g_string_append_printf(string, " (%d%%)", load_progress);
    gchar* title = g_string_free(string, FALSE);
    gtk_window_set_title(window, title);
    g_free(title);
}

static void
link_hover_cb (WebKitWebView* page, const gchar* title, const gchar* link, gpointer data)
{
    if(link)
        gtk_window_set_title((GtkWindow*)main_window, link); 
    else
        update_title((GtkWindow*)main_window);
}

static void
title_change_cb (WebKitWebView* web_view, WebKitWebFrame* web_frame, const gchar* title, gpointer data)
{
    if(main_title)
        g_free(main_title);
    main_title = g_strdup(title);
    update_title((GtkWindow*)main_window);
}

static void
progress_change_cb (WebKitWebView* page, gint progress, gpointer data)
{
    load_progress = progress;
#ifndef NO_FANCY_FUNCTIONS
    gtk_entry_set_progress_fraction((GtkEntry*)uri_entry, load_progress == 100 ? 0 : (double)load_progress / 100);
#endif
    update_title((GtkWindow*)main_window);
}

static void
load_commit_cb (WebKitWebView* page, WebKitWebFrame* frame, gpointer data)
{
    GdkColor color;
    const gchar* uri = webkit_web_frame_get_uri(frame);
    if(uri) {
        gtk_entry_set_text((GtkEntry*)uri_entry, uri);
        if(g_str_has_prefix(uri, "https://")) {
#ifndef NO_FANCY_FUNCTIONS
            gtk_entry_set_icon_from_stock((GtkEntry*)uri_entry, GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_DIALOG_AUTHENTICATION);
#endif
            gdk_color_parse("#b0ff00", &color);
            gtk_widget_modify_base((GtkWidget*)uri_entry, GTK_STATE_NORMAL, &color);
        } else {
#ifndef NO_FANCY_FUNCTIONS
            gtk_entry_set_icon_from_pixbuf((GtkEntry*)uri_entry, GTK_ENTRY_ICON_SECONDARY, NULL);
#endif
            gtk_widget_modify_base((GtkWidget*)uri_entry, GTK_STATE_NORMAL, NULL);
        }
    }
}

static void
load_finished_cb (WebKitWebView* page, WebKitWebFrame* frame, gpointer data)
{
    webkit_web_view_execute_script(page, JS_SETUP);
}

static void
destroy_cb (GtkWidget* widget, gpointer data)
{
    gtk_main_quit();
}

/*
static void
icon_loaded_cb (WebKitWebView* page, gpointer data)
{
    gtk_entry_set_icon_from_gicon((GtkEntry*)uri_entry, GTK_ENTRY_ICON_PRIMARY, data);
}
*/

static gboolean
key_press_uri_entry_cb (WebKitWebView* page, GdkEventKey* event)
{
    if(event->type == GDK_KEY_PRESS && event->keyval == GDK_Escape) {
        /* clean search results if any */
        webkit_web_view_unmark_text_matches(web_view);
        /* revert to url bar */
        gtk_entry_set_text((GtkEntry*)uri_entry, webkit_web_view_get_uri(web_view));
#ifndef NO_FANCY_FUNCTIONS
        /* remove icon */
        gtk_entry_set_icon_from_pixbuf((GtkEntry*)uri_entry, GTK_ENTRY_ICON_PRIMARY, NULL);
#endif
        /* focus webview */
        gtk_widget_grab_focus((GtkWidget*)web_view);
        mode = MODE_NORMAL;
        target = TARGET_CURRENT;
        return (gboolean)TRUE;
    }
    return (gboolean)FALSE;
}

static gboolean
console_message_cb (WebKitWebView* page, gchar* message, gint line, gchar* source_id, gpointer user_data)
{
    if(strncmp(message, "hintmode_off", strlen(message)) == 0 || strncmp(message, "insertmode_off", strlen(message)) == 0) {
        mode = MODE_NORMAL;
        return (gboolean)TRUE;
    } else if(strcmp(message, "insertmode_on") == 0) {
        mode = MODE_INSERT;
        return (gboolean)TRUE;
    }
    return (gboolean)FALSE;
} 

static int
open_from_clipboard (GtkClipboard* c)
{
    const gchar *uri = gtk_clipboard_wait_for_text(c);
    if(uri && (g_str_has_prefix(uri, "http://") || g_str_has_prefix(uri, "https://"))) {
        webkit_web_view_load_uri(web_view, uri);
        return TRUE;
    }
    return FALSE;
}

static gboolean
navigation_request_cb (WebKitWebView* page, GObject* arg1, GObject* arg2, gpointer user_data)
{
    const char* url;
    if(target == TARGET_NEW) {
        url = webkit_network_request_get_uri((WebKitNetworkRequest*)arg2);
        exec(url);
        target = TARGET_CURRENT;
        return (gboolean)TRUE;
    }
    return (gboolean)FALSE;
}

void
exec(const char *param)
{
    GString* cmdline = g_string_new("");
    g_string_append_printf(cmdline, "%s %s", cmd, param);
    g_spawn_command_line_async(cmdline->str, NULL);
    g_string_free(cmdline, TRUE);
}

static gboolean
new_window_cb (WebKitWebView *web_view, WebKitWebFrame *frame, WebKitNetworkRequest *request, WebKitWebNavigationAction *n, WebKitWebPolicyDecision *p, gpointer user_data)
{
    const gchar* url = webkit_network_request_get_uri (request);
    exec(url);
    return (gboolean)FALSE;
}

static gboolean
mimetype_decision_cb (WebKitWebView *page, WebKitWebFrame *frame, WebKitNetworkRequest *request, const gchar* mime_type, WebKitWebPolicyDecision* decision, gpointer user_data)
{
    if(webkit_web_view_can_show_mime_type(page, mime_type))
        return (gboolean)FALSE;
    webkit_web_policy_decision_download(decision);
    webkit_web_view_stop_loading(page);
    return (gboolean)TRUE;
}

static gboolean
download_request_cb (WebKitWebView *web_view, GObject *download, gpointer user_data) {
    const gchar* url = webkit_download_get_uri((WebKitDownload*)download);
    GString* cmdline = g_string_new("");
    g_string_append_printf(cmdline, "%s '%s'", DOWNLOADER, url);
    g_spawn_command_line_async(cmdline->str, NULL);
    g_string_free(cmdline, TRUE);
    return (gboolean)FALSE;
}

static gboolean
button_release_cb (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    const gchar *xcontent = gtk_clipboard_wait_for_text(xclipboard);
    const gchar *content = gtk_clipboard_wait_for_text(clipboard);
    if(event->type == GDK_BUTTON_RELEASE && event->button == 2) {
        if(xcontent) {
            gtk_clipboard_set_text(clipboard, xcontent, -1);
            webkit_web_view_paste_clipboard(web_view);
            gtk_clipboard_set_text(clipboard, content, -1);
        } else
            webkit_web_view_paste_clipboard(web_view);
        return (gboolean)TRUE;
    }
    return (gboolean)FALSE;
}

static void
uri_last_number (int count, int direction)
{
    const gchar *uri = webkit_web_view_get_uri(web_view);
    int diff = (count ? count : 1) * direction;
    GRegex *regex;
    GMatchInfo *match_info;
    const gchar *prev;
    int number;
    GString *url = g_string_new("");

    regex = g_regex_new("^(.+?)([0-9]+)$", 0, 0, NULL);
    if(g_regex_match(regex, uri, 0, &match_info)) {
        prev = g_match_info_fetch(match_info, 1);
        number = g_ascii_strtoll(g_match_info_fetch(match_info, 2), NULL, 0) + diff;
        g_string_printf(url, "%s%d", prev, number);
        webkit_web_view_load_uri(web_view, url->str);
    }
    g_string_free(url, TRUE);
    g_match_info_free(match_info);
    g_regex_unref(regex);
}

static void
uri_nth_subdir (int count)
{
    const gchar *uri = webkit_web_view_get_uri(web_view);
    GRegex *regex;
    GMatchInfo *match_info;
    GString *pattern = g_string_new("");

    g_string_printf(pattern, "^([a-z]+://.+?)((?:/[^/]+){%d})$", count ? count : 1);
    regex = g_regex_new(pattern->str, 0, 0, NULL);
    if(g_regex_match(regex, uri, 0, &match_info))
        webkit_web_view_load_uri(web_view, g_match_info_fetch(match_info, 1));
    g_string_free(pattern, TRUE);
    g_match_info_free(match_info);
    g_regex_unref(regex);
}

static void
uri_root ()
{
    const gchar *uri = webkit_web_view_get_uri(web_view);
    GRegex *regex;
    GMatchInfo *match_info;

    regex = g_regex_new("^([a-z]+://[^/]+)", 0, 0, NULL);
    if(g_regex_match(regex, uri, 0, &match_info))
        webkit_web_view_load_uri(web_view, g_match_info_fetch(match_info, 1));
    g_match_info_free(match_info);
    g_regex_unref(regex);
}

static void
toggle_setting (const gchar* setting)
{
    gboolean value;
    WebKitWebSettings* settings = webkit_web_view_get_settings(web_view);
    g_object_get((GObject*)settings, setting, &value, NULL);
    g_object_set((GObject*)settings, setting, !value, NULL);
    webkit_web_view_set_settings(web_view, settings);
}

static gboolean
key_press_cb (WebKitWebView* page, GdkEventKey* event)
{
    const gchar* txt;
/*
    Vimperator modes

    In normal mode we will listen for commands (and numbers for multiple execution)
    Maybe cmdline mode is invoked via ":" or a shortcut cmd
    If anyone clicks at an object we will enter insert mode

*/
    //if(event->type != GDK_KEY_PRESS)
    //    return (gboolean)FALSE;
    if(mode == MODE_SEARCH && key_press_uri_entry_cb(page, event))
        return (gboolean)TRUE;
    if(mode == MODE_HINTS) {
        if(event->keyval == GDK_Escape) {
            webkit_web_view_execute_script( web_view, "clear();");
            target = TARGET_CURRENT;
        }
        return (gboolean)FALSE;
    }
    if(mode == MODE_INSERT) {
        if(event->keyval == GDK_Escape) {
            webkit_web_view_execute_script(web_view, "clearfocus()");
            mode = MODE_NORMAL;
            return key_press_cb(page, event);
        } else
            return (gboolean)FALSE;
    } else {
        if(event->keyval >= GDK_0 && event->keyval <= GDK_9) {
            count = (count ? count * 10 : 0) + (event->keyval - GDK_0);
            modkey = '\0';
        } else if(event->state == GDK_CONTROL_MASK) {
            switch(event->keyval) {
                case GDK_a:
                    uri_last_number(count, 1);
                    break;
                case GDK_b:
                    do webkit_web_view_move_cursor(web_view, GTK_MOVEMENT_PAGES, -1);
                    while(--count > 0);
                    break;
                case GDK_e:
                    do webkit_web_view_move_cursor(web_view, GTK_MOVEMENT_DISPLAY_LINES, 1);
                    while(--count > 0);
                    break;
                case GDK_f:
                    do webkit_web_view_move_cursor(web_view, GTK_MOVEMENT_PAGES, 1);
                    while(--count > 0);
                    break;
                case GDK_c:
                    webkit_web_view_stop_loading(web_view);
                    break;
                case GDK_i: /* back */
                    webkit_web_view_go_back_or_forward(web_view, (gint)(-1 * (count ? count : 1)));
                    break;
                case GDK_o: /* fwd */
                    webkit_web_view_go_back_or_forward(web_view, (gint)(count ? count : 1));
                    break;
                case GDK_y:
                    do webkit_web_view_move_cursor(web_view, GTK_MOVEMENT_DISPLAY_LINES, -1);
                    while(--count > 0);
                    break;
                case GDK_x:
                    uri_last_number(count, -1);
                    break;
                default:
                    return (gboolean)FALSE;
            }
            count = 0;
            modkey = '\0';
        } else if(event->state == 0 || event->state == GDK_SHIFT_MASK) {
            switch(event->keyval) {
                case GDK_numbersign: /* # */
                    modkey = GDK_numbersign;
                    return (gboolean)TRUE;
                    break;
                case GDK_f:
                    mode = MODE_HINTS;
                    webkit_web_view_execute_script(web_view, JS_ENABLE_HINTS);
                    break;
                case GDK_F:
                    mode = MODE_HINTS;
                    target = TARGET_NEW;
                    webkit_web_view_execute_script(web_view, JS_ENABLE_HINTS);
                    break;
                case GDK_g:
                    if(modkey == GDK_g)
                        webkit_web_view_move_cursor(web_view, GTK_MOVEMENT_BUFFER_ENDS, -1);
                    else {
                        modkey = GDK_g;
                        return (gboolean)TRUE;
                    }
                    break;
                case GDK_G:
                    webkit_web_view_move_cursor(web_view, GTK_MOVEMENT_BUFFER_ENDS, 1);
                    break;
                case GDK_h:
                    if(modkey == GDK_g)
                        webkit_web_view_load_uri(web_view, STARTPAGE);
                    else
                        do webkit_web_view_move_cursor(web_view, GTK_MOVEMENT_VISUAL_POSITIONS, -1);
                        while(--count > 0);
                    break;
                case GDK_H:
                    if(modkey == GDK_g)
                        exec(STARTPAGE);
                    else
                        webkit_web_view_go_back_or_forward(web_view, (gint)(-1 * (count ? count : 1)));
                    break;
                case GDK_j:
                    if(modkey == GDK_numbersign)
                        toggle_setting("enable-scripts");
                    else
                        do webkit_web_view_move_cursor(web_view, GTK_MOVEMENT_DISPLAY_LINES, 1);
                        while(--count > 0);
                    break;
                case GDK_k:
                    do webkit_web_view_move_cursor(web_view, GTK_MOVEMENT_DISPLAY_LINES, -1);
                    while(--count > 0);
                    break;
                case GDK_l:
                    do webkit_web_view_move_cursor(web_view, GTK_MOVEMENT_VISUAL_POSITIONS, 1);
                    while(--count > 0);
                    break;
                case GDK_L: /* fwd */
                    webkit_web_view_go_back_or_forward(web_view, (gint)(count ? count : 1));
                    break;
                case GDK_space:
                    do webkit_web_view_move_cursor(web_view, GTK_MOVEMENT_PAGES, (event->state == GDK_SHIFT_MASK ? -1 : 1));
                    while(--count > 0);
                    break;
                case GDK_r:
                    webkit_web_view_reload(web_view);
                    break;
                case GDK_R:
                    webkit_web_view_reload_bypass_cache(web_view);
                    break;
                case GDK_slash: /* set up search mode */
#ifndef NO_FANCY_FUNCTIONS
                    gtk_entry_set_icon_from_stock((GtkEntry*)uri_entry, GTK_ENTRY_ICON_PRIMARY, GTK_STOCK_FIND);
#endif
                    gtk_entry_set_text((GtkEntry*)uri_entry, "");
                    gtk_widget_grab_focus((GtkWidget*) uri_entry);
                    mode = MODE_SEARCH;
                    break;
                case GDK_o: /* insert url */
                    if(modkey == GDK_z)
                        zoom(count, (gboolean)FALSE, (gboolean)FALSE);
                    else {
                        gtk_entry_set_text((GtkEntry*)uri_entry, "http://");
                        gtk_widget_grab_focus((GtkWidget*) uri_entry);
                        gtk_editable_set_position((GtkEditable*)uri_entry, -1);
                    }
                    break;
                case GDK_d:
                    gtk_main_quit();
                    break;
                case GDK_n: /* next match(es) */
                    if(mode == MODE_SEARCH)
                        do find_next((gboolean)TRUE);
                        while(--count > 0);
                    else
                        return (gboolean)FALSE;
                    break;
                case GDK_N: /* prev match(es) */
                    if(mode == MODE_SEARCH)
                        do find_next((gboolean)FALSE);
                        while(--count > 0);
                    else
                        return (gboolean)FALSE;
                    break;
                /*
                    full zooming
                */
                case GDK_I:
                    if(modkey == GDK_z)
                        zoom(count, (gboolean)TRUE, (gboolean)TRUE);
                    else
                         return (gboolean)FALSE;
                    break;
                case GDK_O:
                    if(modkey == GDK_z)
                        zoom(count, (gboolean)FALSE, (gboolean)TRUE);
                    else { /* append to url */
                        gtk_widget_grab_focus((GtkWidget*) uri_entry);
                        gtk_editable_set_position((GtkEditable*)uri_entry, -1);
                    }
                    break;
                case GDK_Z:
                    if(modkey == GDK_z) {
                        webkit_web_view_set_full_content_zoom(web_view, (gboolean)TRUE);
                        webkit_web_view_set_zoom_level(web_view, (count ? ((float)count)/100.0 : 1.0));
                    } else
                        return (gboolean)FALSE;
                    break;
                /*
                    text zooming
                */
                case GDK_i:
                    if(modkey == GDK_numbersign)
                        toggle_setting("auto-load-images");
                    else if(modkey == GDK_z)
                        zoom(count, (gboolean)TRUE, (gboolean)FALSE);
                    else
                        return (gboolean)FALSE;
                    break;
                case GDK_plus:
                    zoom(count, (gboolean)TRUE, (gboolean)FALSE);
                    break;
                case GDK_minus:
                    zoom(count, (gboolean)FALSE, (gboolean)FALSE);
                    break;
                case GDK_z:
                    if(modkey == GDK_z) {
                        webkit_web_view_set_full_content_zoom(web_view, (gboolean)FALSE);
                        webkit_web_view_set_zoom_level(web_view, (count ? ((float)count)/100.0 : 1.0)); 
                    } else {
                        modkey = GDK_z;
                        return (gboolean)TRUE;
                    }
                    break;
                case GDK_y:
                    txt = webkit_web_view_get_uri(web_view);
                    gtk_clipboard_set_text(xclipboard, txt, -1);
                    gtk_clipboard_set_text(clipboard, txt, -1);
                    break;
                case GDK_Y:
                    webkit_web_view_copy_clipboard(web_view);
                    break;
                case GDK_p:
                    if(!open_from_clipboard(xclipboard))
                        open_from_clipboard(clipboard);
                    break;
                case GDK_P:
                    target = TARGET_NEW;
                    if(!open_from_clipboard(xclipboard))
                        open_from_clipboard(clipboard);
                    target = TARGET_CURRENT;
                    break;
                case GDK_u:
                    if(modkey == GDK_g)
                        uri_nth_subdir(count);
                    else
                        return (gboolean)FALSE;
                    break;
                case GDK_U:
                    if(modkey == GDK_g)
                        uri_root();
                    else
                        return (gboolean)FALSE;
                    break;
                /* tabs */
                case GDK_t:
                    target = TARGET_NEW;
                    gtk_entry_set_text((GtkEntry*)uri_entry, "http://");
                    gtk_widget_grab_focus((GtkWidget*) uri_entry);
                    gtk_editable_set_position((GtkEditable*)uri_entry, -1);
                    break;
                default:
                    return (gboolean)FALSE;
            }
            count = 0;
            modkey = '\0';
        }
    }

    return (gboolean)TRUE;

//        switch(event->hardware_keycode) {
//            case 233: /* XF86XK_Forward */
//                webkit_web_view_go_forward(web_view);
//                return (gboolean)TRUE;
//            case 234: /* XF86XK_Back */
//                webkit_web_view_go_back(web_view);
//                return (gboolean)TRUE;
//        }
}

static GtkWidget*
create_browser ()
{
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow*)scrolled_window, GTK_POLICY_NEVER, GTK_POLICY_NEVER);

    web_view = (WebKitWebView*)webkit_web_view_new();

    WebKitWebSettings* settings = webkit_web_settings_new();
    g_object_set((GObject*)settings, "user-stylesheet-uri", SETTING_USER_CSS_FILE, NULL);
    g_object_get((GObject*)settings, "zoom-step", &zoomstep, NULL);
    //g_object_set (G_OBJECT(settings), "enable-developer-extras", TRUE, NULL);

    webkit_web_view_set_settings(web_view, settings);

    gtk_container_add((GtkContainer*)scrolled_window, (GtkWidget*)web_view);

    g_signal_connect((GObject*)web_view, "title-changed", (GCallback)title_change_cb, web_view);
    g_signal_connect((GObject*)web_view, "load-progress-changed", (GCallback)progress_change_cb, web_view);
    g_signal_connect((GObject*)web_view, "hovering-over-link", (GCallback)link_hover_cb, web_view);
    g_signal_connect((GObject*)web_view, "load-committed", (GCallback)load_commit_cb, web_view);
    g_signal_connect((GObject*)web_view, "load-finished", (GCallback)load_finished_cb, web_view);
    g_signal_connect((GObject*)web_view, "navigation-requested", (GCallback)navigation_request_cb, web_view);
    g_signal_connect((GObject*)web_view, "new-window-policy-decision-requested", (GCallback)new_window_cb, web_view);
    g_signal_connect((GObject*)web_view, "download-requested", (GCallback)download_request_cb, web_view);
    g_signal_connect((GObject*)web_view, "mime-type-policy-decision-requested", (GCallback)mimetype_decision_cb, web_view);
    g_signal_connect((GObject*)web_view, "key-press-event", (GCallback)key_press_cb, web_view);
    g_signal_connect((GObject*)web_view, "button-release-event", (GCallback)button_release_cb, web_view);
    /* hack for hinting mode */
    g_signal_connect ((GObject*)web_view, "console-message", (GCallback)console_message_cb, web_view);

    return scrolled_window;
}

static GtkWidget*
create_toolbar ()
{
    GtkWidget* toolbar = gtk_toolbar_new();

    gtk_toolbar_set_orientation((GtkToolbar*)toolbar, GTK_ORIENTATION_HORIZONTAL);
    gtk_toolbar_set_style((GtkToolbar*)toolbar, GTK_TOOLBAR_BOTH_HORIZ);

    GtkToolItem* item;

    /* The URL entry */
    item = gtk_tool_item_new();
    gtk_tool_item_set_expand(item, TRUE);
    uri_entry = gtk_entry_new();
    /* use new fraction prop of GtkEntry (requires gtk >= 2.16) */
    gtk_container_add((GtkContainer*)item, uri_entry);
    g_signal_connect((GObject*)uri_entry, "activate", (GCallback)activate_uri_entry_cb, NULL);
    g_signal_connect((GObject*)uri_entry, "key-press-event", (GCallback)key_press_uri_entry_cb, NULL);

    gtk_toolbar_insert((GtkToolbar*)toolbar, item, -1);

    return toolbar;
}

static GtkWidget*
create_window ()
{
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size((GtkWindow*)window, 800, 600);
    gtk_widget_set_name(window, "GtkLauncher");
    g_signal_connect((GObject*)window, "destroy", (GCallback)destroy_cb, NULL);

    return window;
}
/*
static WebKitWebView*
embed_inspector (WebKitWebInspector *web_inspector, WebKitWebView *target, gpointer user_data)
{
    WebKitWebView* webview;

    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow*)scrolled_window, GTK_POLICY_NEVER, GTK_POLICY_NEVER);

    webview = (WebKitWebView*)webkit_web_view_new();
    gtk_container_add((GtkContainer*)scrolled_window, (GtkWidget*)web_view);

    gtk_box_pack_start((GtkBox*)vbox, (GtkWidget*)webview, TRUE, TRUE, 0);
    return webview;
}
*/
int
main (int argc, char* argv[])
{
    int i;

    cmd = argv[0];
    //WebKitWebInspector *inspector;
    gtk_init(&argc, &argv);
    if(!g_thread_supported())
        g_thread_init(NULL);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start((GtkBox*)vbox, create_toolbar(), FALSE, FALSE, 0);
    gtk_box_pack_start((GtkBox*)vbox, create_browser(), TRUE, TRUE, 0);
    //inspector = webkit_web_view_get_inspector(WEBKIT_WEB_VIEW(web_view));

    xclipboard = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
    clipboard = gtk_clipboard_get(GDK_NONE);
    main_window = create_window();
    gtk_container_add((GtkContainer*)main_window, vbox);

    mode = MODE_NORMAL;
    target = TARGET_CURRENT;

    gchar* uri = (gchar*) (argc > 1 ? argv[1] : STARTPAGE);
    webkit_web_view_load_uri(web_view, uri);

    for(i = 2; i < argc; i++)
        exec(argv[i]);
    //g_signal_connect((GObject*)inspector, "inspect-web-view", G_CALLBACK(embed_inspector), NULL);

    gtk_widget_grab_focus((GtkWidget*)web_view);
    gtk_widget_show_all(main_window);
    gtk_main();

    return 0;
}
