import java.util.*;
import java.awt.*;

import javax.swing.*;

import com.sun.jna.platform.DesktopWindow;

import com.sun.jna.platform.win32.WinDef.*;

import com.formdev.flatlaf.FlatDarculaLaf;

public class Main {

    static JMenuBar buildDefaultMenuBar() {
        JMenuBar menuBar = new JMenuBar();

        JMenu fileMenu = new JMenu("File");
        JMenu viewMenu = new JMenu("View");
        JMenu helpMenu = new JMenu("Help");

        JMenuItem newItem = new JMenuItem("New");
        JMenuItem openItem = new JMenuItem("Open");
        JMenuItem exitItem = new JMenuItem("Exit");

        JMenuItem lightThemeItem = new JMenuItem("Light Theme");
        JMenuItem darkThemeItem = new JMenuItem("Dark Theme");

        JMenuItem aboutItem = new JMenuItem("About");

        fileMenu.add(newItem);
        fileMenu.add(openItem);
        fileMenu.addSeparator();
        fileMenu.add(exitItem);

        viewMenu.add(lightThemeItem);
        viewMenu.add(darkThemeItem);

        helpMenu.add(aboutItem);

        menuBar.add(fileMenu);
        menuBar.add(viewMenu);
        menuBar.add(helpMenu);

        return menuBar;
    }

    static JPanel buildQuickBarPanel() {
        JPanel layout = new JPanel();

        /*
        JPanel leftPanel = new JPanel(new GridBagLayout());
        JPanel rightPanel = new JPanel(new GridBagLayout());

        GridBagConstraints gbc = new GridBagConstraints();

        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.weightx = 0.3;
        gbc.weighty = 1.0;
        gbc.fill = GridBagConstraints.BOTH;

        leftPanel.setBackground(new Color(128, 0, 0));

        layout.add(leftPanel, gbc);

        gbc.gridx = 1;
        gbc.gridy = 0;
        gbc.weightx = 0.7;
        gbc.weighty = 1.0;
        gbc.fill = GridBagConstraints.BOTH;

        rightPanel.setBackground(new Color(0, 128, 0));

        layout.add(rightPanel, gbc);
        */

        return layout;
    }

    static JPanel createViewportPanel() {
        JPanel viewport = new JPanel();

        viewport.setVisible(true);
        viewport.setDoubleBuffered(false);

        java.util.List<DesktopWindow> windows = com.sun.jna.platform.WindowUtils.getAllWindows(false);

        HWND hwnd = null;

        if (!windows.isEmpty()) {
            hwnd = windows.getFirst().getHWND();
        }

        if (hwnd == null) {
            throw new NullPointerException();
        }

        new Thread(() -> {
            while (true) {
                // native render logic using HWND
                // e.g. DirectXPresent(hwnd)
            }
        }).start();

        return viewport;
    }

    public static class MainFrame extends JFrame {
        public MainFrame() {
            setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            setSize(1920, 1080);
            //setLayout(new GridBagLayout());

            JMenuBar rootMenuBar = buildDefaultMenuBar();

            setJMenuBar(rootMenuBar);

            JPanel quickBar = buildQuickBarPanel();

            quickBar.setBackground(new Color(128, 0, 0));

            JPanel hierarchy = buildQuickBarPanel();

            hierarchy.setBackground(new Color(0, 128, 0));

            JSplitPane secondHorizontalSplit = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, hierarchy, null);

            secondHorizontalSplit.setDividerLocation(1000);

            JSplitPane firstHorizontalSplit = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, quickBar, secondHorizontalSplit);

            firstHorizontalSplit.setDividerLocation(100);
            firstHorizontalSplit.setResizeWeight(0.0F);

            add(firstHorizontalSplit);
        }
    }

    public static void main(String[] args) {
        NativeContext nativeContext = new NativeContext();

        nativeContext.createContext();
        nativeContext.updateContext();
        nativeContext.destroyContext();

        try {
            UIManager.setLookAndFeel(new FlatDarculaLaf());
        } catch (UnsupportedLookAndFeelException e) {
            throw new RuntimeException(e);
        }

        RepaintManager.currentManager(null).setDoubleBufferingEnabled(false);

        SwingUtilities.invokeLater(() -> new MainFrame().setVisible(true));
    }

}