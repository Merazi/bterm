#include "config.h"
#include <vte/vte.h>

static void child_ready(VteTerminal *terminal, GPid pid, GError *error, gpointer user_data)
{
    if (!terminal) return;
    if (pid == -1) gtk_main_quit();
}

static gboolean on_title_changed(GtkWidget *terminal, gpointer user_data)
{
    GtkWindow *window = user_data;
    gtk_window_set_title(window,
            vte_terminal_get_window_title(VTE_TERMINAL(terminal))?:"bterm");
    return TRUE;
}

int main(int argc, char *argv[])
{
    GtkWidget *window, *terminal;

    /* Initialise GTK, the window and the terminal */
    gtk_init(&argc, &argv);
    terminal = vte_terminal_new();
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "bterm");

    /* fonts */
    PangoFontDescription * desc = pango_font_description_from_string(FONT);
    vte_terminal_set_font(VTE_TERMINAL(terminal), desc);

    /* behaviour */
    vte_terminal_set_scrollback_lines(VTE_TERMINAL(terminal), SCROLL_BACK_LINES);
    vte_terminal_set_scroll_on_output(VTE_TERMINAL(terminal), SCROLL_ON_OUTPUT);
    vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(terminal), SCROLL_ON_KEYSTROKE);
    vte_terminal_set_mouse_autohide(VTE_TERMINAL(terminal), HIDE_MOUSE_WHEN_TYPING);

    /* keyboard shortcuts */


    /* Start a new shell */
    gchar **envp = g_get_environ();
    gchar **command = (gchar *[]){g_strdup(g_environ_getenv(envp, "SHELL")), argv[1], NULL };
    g_strfreev(envp);
    vte_terminal_spawn_async(VTE_TERMINAL(terminal),
            VTE_PTY_DEFAULT,
            NULL,         /* working directory  */
            command,      /* command */
            NULL,         /* environment */
            0,            /* spawn flags */
            NULL, NULL,   /* child setup */
            NULL,         /* child pid */
            -1,           /* timeout */
            NULL,         /* cancellable */
            child_ready,  /* callback */
            NULL);        /* user_data */

    /* Connect some signals */
    g_signal_connect(terminal, "window-title-changed", 
            G_CALLBACK(on_title_changed), GTK_WINDOW(window));
    g_signal_connect(window, "delete-event", gtk_main_quit, NULL);
    g_signal_connect(terminal, "child-exited", gtk_main_quit, NULL);

    /* Put widgets together and run the main loop */
    gtk_container_add(GTK_CONTAINER(window), terminal);
    gtk_widget_show_all(window);
    gtk_main();
}
