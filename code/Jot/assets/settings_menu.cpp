#include "settings_menu.h"
#include "config.h"
#include "text.h"
#include "cloud_sync.h"
#include "ui_display.h"
#include <ncurses.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
void showConnectionTest();
void showSyncTypeMenu();
void showSettingsMenu();
// Implementation will be copied from main.cpp lines 359-810
void showConnectionTest() {
    clear();
    attron(A_BOLD);
    mvprintw(0, 0, TEXT_CONNECTION_TEST_HEADER);
    attroff(A_BOLD);
    drawSeparator(1);

    // Check if sync is enabled and remote is set
    if (!g_cloudSyncEnabled) {
        mvprintw(3, 2, TEXT_CONNECTION_TEST_SYNC_DISABLED);
        mvprintw(LINES - 2, 0, TEXT_CONNECTION_PRESS_KEY);
        refresh();
        getch();
        return;
    }

    if (g_currentRemote.empty()) {
        mvprintw(3, 2, TEXT_CONNECTION_TEST_NO_REMOTE);
        mvprintw(LINES - 2, 0, TEXT_CONNECTION_PRESS_KEY);
        refresh();
        getch();
        return;
    }

    // Test connection
    mvprintw(3, 2, "%s%s", TEXT_CONNECTION_TEST_TESTING, g_currentRemote.c_str());
    refresh();
    sleep(1);

    bool connected = testCloudConnection();

    clear();
    attron(A_BOLD);
    mvprintw(0, 0, TEXT_CONNECTION_TEST_HEADER);
    attroff(A_BOLD);
    drawSeparator(1);

    if (connected) {
        attron(A_BOLD);
        mvprintw(3, 2, TEXT_CONNECTION_TEST_SUCCESS);
        attroff(A_BOLD);
        mvprintw(5, 2, "Remote: %s", g_currentRemote.c_str());
        mvprintw(6, 2, "Status: Connected and ready");
        mvprintw(LINES - 2, 0, TEXT_CONNECTION_PRESS_KEY);
    } else {
        attron(A_BOLD);
        mvprintw(3, 2, TEXT_CONNECTION_TEST_FAILED);
        attroff(A_BOLD);
        mvprintw(5, 2, "Remote: %s", g_currentRemote.c_str());
        mvprintw(6, 2, "Status: Authentication error or network issue");
        mvprintw(8, 2, TEXT_CONNECTION_FIX_PROMPT);
    }

    refresh();

    if (!connected) {
        // Offer to fix
        int ch = getch();
        if (ch == 'y' || ch == 'Y' || ch == '\n' || ch == KEY_ENTER) {
            // Show fix wizard
            clear();
            attron(A_BOLD);
            mvprintw(0, 0, TEXT_FIX_WIZARD_HEADER);
            attroff(A_BOLD);
            drawSeparator(1);

            mvprintw(3, 2, TEXT_FIX_WIZARD_PROBLEM);
            mvprintw(4, 2, "Remote: %s", g_currentRemote.c_str());

            drawSeparator(6);
            mvprintw(7, 2, TEXT_FIX_WIZARD_SOLUTION);
            mvprintw(9, 2, TEXT_FIX_WIZARD_OPTION_1);
            mvprintw(10, 2, TEXT_FIX_WIZARD_RECONNECT_INFO);
            mvprintw(12, 2, TEXT_FIX_WIZARD_OPTION_2);
            mvprintw(13, 2, TEXT_FIX_WIZARD_RECONFIG_INFO);
            mvprintw(15, 2, TEXT_FIX_WIZARD_OPTION_3);

            drawSeparator(17);
            mvprintw(18, 0, "Select option (1, 2, or 3): ");
            refresh();

            int option = getch();

            if (option == '1') {
                // Quick reconnect
                def_prog_mode();
                endwin();

                std::cout << "\n" << TEXT_FIX_WIZARD_HEADER << "\n";
                std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
                std::cout << "Reconnecting to: " << g_currentRemote << "\n\n";
                std::cout << TEXT_FIX_WIZARD_INSTRUCTIONS << "\n";
                std::cout << TEXT_FIX_WIZARD_BROWSER << "\n\n";

                std::string cmd = "rclone config reconnect \"" + g_currentRemote + "\"";
                int result = system(cmd.c_str());

                if (result != 0) {
                    std::cout << "\nReconnect not available, trying full config...\n";
                    std::cout << "In the menu, select your remote and choose 'reconnect'\n\n";
                    system("rclone config");
                }

                std::cout << "\n" << TEXT_FIX_WIZARD_TESTING << "\n";
                sleep(1);

                reset_prog_mode();
                refresh();

                // Test again
                if (testCloudConnection()) {
                    clear();
                    attron(A_BOLD);
                    mvprintw(LINES/2, (COLS - 40)/2, TEXT_FIX_WIZARD_SUCCESS);
                    attroff(A_BOLD);
                    mvprintw(LINES/2 + 2, (COLS - 40)/2, TEXT_CONNECTION_PRESS_KEY);
                    refresh();
                    getch();
                } else {
                    clear();
                    mvprintw(LINES/2, 2, TEXT_FIX_WIZARD_FAILED);
                    mvprintw(LINES/2 + 2, 2, "You may need to delete and recreate the remote.");
                    mvprintw(LINES/2 + 3, 2, TEXT_CONNECTION_PRESS_KEY);
                    refresh();
                    getch();
                }
            } else if (option == '2') {
                // Full reconfiguration
                def_prog_mode();
                endwin();

                std::cout << "\n" << TEXT_FIX_WIZARD_HEADER << "\n";
                std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
                std::cout << "Opening rclone configuration...\n\n";
                std::cout << "Steps:\n";
                std::cout << "1. Select your remote: " << g_currentRemote << "\n";
                std::cout << "2. Choose 'Edit existing remote' or 'reconnect'\n";
                std::cout << "3. Follow the authentication prompts\n";
                std::cout << "4. " << TEXT_FIX_WIZARD_BROWSER << "\n\n";
                std::cout << "Press ENTER to continue...";
                std::cin.get();

                system("rclone config");

                std::cout << "\n" << TEXT_FIX_WIZARD_TESTING << "\n";
                sleep(1);

                reset_prog_mode();
                refresh();

                // Test again
                if (testCloudConnection()) {
                    clear();
                    attron(A_BOLD);
                    mvprintw(LINES/2, (COLS - 40)/2, TEXT_FIX_WIZARD_SUCCESS);
                    attroff(A_BOLD);
                    mvprintw(LINES/2 + 2, (COLS - 40)/2, TEXT_CONNECTION_PRESS_KEY);
                    refresh();
                    getch();
                } else {
                    clear();
                    mvprintw(LINES/2, 2, TEXT_FIX_WIZARD_FAILED);
                    mvprintw(LINES/2 + 2, 2, TEXT_CONNECTION_PRESS_KEY);
                    refresh();
                    getch();
                }
            }
            // Option 3 or any other key: cancel
        }
    } else {
        getch();
    }
}

// Sync type selection
void showSyncTypeMenu() {
    clear();
    attron(A_BOLD);
    mvprintw(0, 0, TEXT_SYNC_TYPE_HEADER);
    attroff(A_BOLD);
    drawSeparator(1);

    const char* currentType = (g_syncType == SYNC_TYPE_FULL) ? TEXT_SYNC_TYPE_FULL : TEXT_SYNC_TYPE_ONE_WAY;
    mvprintw(3, 2, "%s%s", TEXT_SYNC_TYPE_CURRENT, currentType);

    drawSeparator(5);

    attron(A_BOLD);
    mvprintw(7, 2, TEXT_SYNC_TYPE_FULL_DESC);
    attroff(A_BOLD);
    mvprintw(8, 2, TEXT_SYNC_TYPE_FULL_DETAIL);
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(9, 2, TEXT_SYNC_TYPE_FULL_WARNING);
    attroff(COLOR_PAIR(1) | A_BOLD);

    mvprintw(11, 2, "");
    attron(A_BOLD);
    mvprintw(11, 2, TEXT_SYNC_TYPE_ONE_WAY_DESC);
    attroff(A_BOLD);
    mvprintw(12, 2, TEXT_SYNC_TYPE_ONE_WAY_DETAIL);
    attron(A_BOLD);
    mvprintw(13, 2, TEXT_SYNC_TYPE_ONE_WAY_SAFE);
    attroff(A_BOLD);

    drawSeparator(15);
    mvprintw(16, 0, TEXT_SYNC_TYPE_SELECT);
    refresh();

    int ch = getch();
    if (ch == '1') {
        g_syncType = SYNC_TYPE_FULL;
        saveSettings();
    } else if (ch == '2') {
        g_syncType = SYNC_TYPE_ONE_WAY;
        saveSettings();
    }
}

// Settings menu UI
void showSettingsMenu() {
    int selectedOption = 0;
    bool inMenu = true;

    while (inMenu) {
        clear();

        // Header
        attron(A_BOLD);
        mvprintw(0, 0, TEXT_SETTINGS_HEADER);
        attroff(A_BOLD);
        drawSeparator(1);

        // Current settings display
        mvprintw(3, 2, "%s%s", TEXT_SETTINGS_CURRENT_REMOTE,
                 g_currentRemote.empty() ? TEXT_SETTINGS_NONE : g_currentRemote.c_str());
        mvprintw(4, 2, "%s%s", TEXT_SETTINGS_CLOUD_SYNC,
                 g_cloudSyncEnabled ? TEXT_SETTINGS_ENABLED : TEXT_SETTINGS_DISABLED);
        mvprintw(5, 2, "%s%s", TEXT_SETTINGS_AUTO_SYNC,
                 g_autoSyncEnabled ? TEXT_SETTINGS_ON : TEXT_SETTINGS_OFF);
        const char* syncTypeStr = (g_syncType == SYNC_TYPE_FULL) ? TEXT_SYNC_TYPE_FULL : TEXT_SYNC_TYPE_ONE_WAY;
        mvprintw(6, 2, "%s%s", TEXT_SETTINGS_SYNC_TYPE, syncTypeStr);

        drawSeparator(8);
        mvprintw(9, 0, TEXT_SETTINGS_OPTIONS);

        // Menu options
        const char* options[] = {
            TEXT_SETTINGS_OPT_VIEW,
            TEXT_SETTINGS_OPT_STATUS,
            TEXT_SETTINGS_OPT_SWITCH,
            TEXT_SETTINGS_OPT_TOGGLE_CLOUD,
            TEXT_SETTINGS_OPT_TOGGLE_AUTO,
            TEXT_SETTINGS_OPT_SYNC_NOW,
            TEXT_SETTINGS_OPT_SETUP,
            TEXT_SETTINGS_FIX_REMOTE,
            TEXT_SETTINGS_OPT_TEST_CONNECTION,
            TEXT_SETTINGS_OPT_SYNC_TYPE,
            TEXT_SETTINGS_OPT_BACK_NEW
        };

        for (int i = 0; i < 11; i++) {
            if (i == selectedOption) {
                attron(A_REVERSE);
            }
            mvprintw(11 + i, 2, "%s", options[i]);
            if (i == selectedOption) {
                attroff(A_REVERSE);
            }
        }

        drawSeparator(23);
        mvprintw(24, 0, TEXT_SETTINGS_NAV);

        refresh();

        int ch = getch();
        switch(ch) {
            case KEY_UP:
                if (selectedOption > 0) selectedOption--;
                break;
            case KEY_DOWN:
                if (selectedOption < 10) selectedOption++;
                break;
            case '\n':
            case KEY_ENTER:
                switch(selectedOption) {
                    case 0: { // View Remotes
                        def_prog_mode();
                        endwin();

                        std::cout << TEXT_SETTINGS_VIEW_HEADER << "\n";
                        system("rclone listremotes");
                        std::cout << TEXT_SETTINGS_VIEW_CONTINUE;
                        std::cin.get();

                        reset_prog_mode();
                        refresh();
                        break;
                    }
                    case 1: { // Sync Status
                        clear();
                        mvprintw(0, 0, TEXT_SETTINGS_STATUS_CHECKING);
                        refresh();

                        bool connectionOk = false;
                        if (g_cloudSyncEnabled && !g_currentRemote.empty()) {
                            mvprintw(1, 0, TEXT_SETTINGS_TESTING_CONNECTION);
                            refresh();
                            connectionOk = testCloudConnection();
                        }

                        sleep(1);

                        clear();
                        attron(A_BOLD);
                        mvprintw(0, 0, "Sync Status");
                        attroff(A_BOLD);
                        drawSeparator(1);

                        if (!g_cloudSyncEnabled) {
                            mvprintw(3, 2, TEXT_SETTINGS_STATUS_CLOUD_DISABLED);
                        } else {
                            mvprintw(3, 2, "%s%s", TEXT_SETTINGS_STATUS_CONNECTED, g_currentRemote.c_str());
                            std::string cloudPath = g_currentRemote + ":" + RCLONE_PATH;
                            mvprintw(4, 2, "%s%s", TEXT_SETTINGS_STATUS_PATH, cloudPath.c_str());
                            mvprintw(5, 2, "%s%s", TEXT_SETTINGS_STATUS_LOCAL, BASE_DIR.c_str());
                            mvprintw(7, 2, "%s", g_autoSyncEnabled ? TEXT_SETTINGS_STATUS_MODE_AUTO : TEXT_SETTINGS_STATUS_MODE_MANUAL);

                            // Show connection status
                            mvprintw(9, 2, "Connection test: %s", connectionOk ? TEXT_SETTINGS_CONNECTION_OK : TEXT_SETTINGS_CONNECTION_FAILED);
                            if (!connectionOk) {
                                mvprintw(10, 2, TEXT_SETTINGS_CONNECTION_HELP);
                            }
                        }

                        mvprintw(LINES - 2, 0, "Press any key to continue...");
                        refresh();
                        getch();
                        break;
                    }
                    case 2: { // Switch Remote
                        std::vector<std::string> remotes = listRcloneRemotes();
                        if (remotes.empty()) {
                            mvprintw(LINES - 3, 0, TEXT_SETTINGS_NO_REMOTES);
                            clrtoeol();
                            refresh();
                            sleep(2);
                        } else {
                            int remoteIdx = 0;
                            bool selecting = true;

                            while (selecting) {
                                clear();
                                attron(A_BOLD);
                                mvprintw(0, 0, TEXT_SETTINGS_SELECT_REMOTE);
                                attroff(A_BOLD);
                                drawSeparator(1);

                                for (size_t i = 0; i < remotes.size(); i++) {
                                    if (i == remoteIdx) attron(A_REVERSE);
                                    mvprintw(3 + i, 2, "%s", remotes[i].c_str());
                                    if (i == remoteIdx) attroff(A_REVERSE);
                                }

                                mvprintw(LINES - 2, 0, TEXT_SETTINGS_ENTER_SELECT);
                                refresh();

                                int ch2 = getch();
                                if (ch2 == KEY_UP && remoteIdx > 0) remoteIdx--;
                                else if (ch2 == KEY_DOWN && remoteIdx < (int)remotes.size() - 1) remoteIdx++;
                                else if (ch2 == '\n' || ch2 == KEY_ENTER) {
                                    g_currentRemote = remotes[remoteIdx];
                                    saveSettings();
                                    selecting = false;
                                } else if (ch2 == 27) {
                                    selecting = false;
                                }
                            }
                        }
                        break;
                    }
                    case 3: // Toggle Cloud Sync
                        g_cloudSyncEnabled = !g_cloudSyncEnabled;
                        saveSettings();
                        break;
                    case 4: // Toggle Auto Sync
                        g_autoSyncEnabled = !g_autoSyncEnabled;
                        saveSettings();
                        break;
                    case 5: { // Sync Now
                        mvprintw(LINES - 3, 0, TEXT_SETTINGS_SYNCING);
                        clrtoeol();
                        refresh();

                        if (g_cloudSyncEnabled && !g_currentRemote.empty()) {
                            syncToCloud(false); // foreground sync
                            mvprintw(LINES - 3, 0, TEXT_SETTINGS_SYNC_COMPLETE);
                        } else {
                            mvprintw(LINES - 3, 0, TEXT_SETTINGS_SYNC_DISABLED);
                        }
                        clrtoeol();
                        refresh();
                        sleep(2);
                        break;
                    }
                    case 6: { // Setup New Remote
                        def_prog_mode();
                        endwin();

                        std::cout << TEXT_SETTINGS_SETUP_HEADER << "\n";
                        std::cout << TEXT_SETTINGS_SETUP_RUNNING << "\n\n";
                        system("rclone config");

                        std::cout << TEXT_SETTINGS_SETUP_RETURN;
                        std::cin.get();

                        reset_prog_mode();
                        refresh();
                        break;
                    }
                    case 7: { // Fix/Reconnect Remote - Use new wizard
                        if (g_currentRemote.empty()) {
                            mvprintw(LINES - 3, 0, "No remote selected. Use option 3 to select a remote.");
                            clrtoeol();
                            refresh();
                            sleep(2);
                        } else {
                            showConnectionTest(); // This includes the fix wizard
                        }
                        break;
                    }
                    case 8: { // Test Connection
                        showConnectionTest();
                        break;
                    }
                    case 9: { // Change Sync Type
                        showSyncTypeMenu();
                        break;
                    }
                    case 10: // Back
                        inMenu = false;
                        break;
                }
                break;
            case 27: // ESC
            case 'q':
            case 'Q':
                inMenu = false;
                break;
        }
    }
}
