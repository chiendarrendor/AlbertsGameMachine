package TheGameMachine;

import javax.swing.SwingUtilities;


public class ClientApplication {
    private static class Runner implements Runnable {
        static String server = null;
        static String uname = null;
        static String password = null;

        public void run() {
            createAndShowGUI(server,uname,password);
        }
    }


    public static void main(String[] args) {
        Runner runner = new Runner();

        switch (args.length) {
            case 1:
                runner.server = args[0];
                break;
            case 3:
                runner.server = args[0];
                runner.uname = args[1];
                runner.password = args[2];
                break;
            default:
                System.out.println("bad command line: <server name> [username password]");
                System.exit(1);
        }
        SwingUtilities.invokeLater(runner);
    }



    // ClientPanel is a leftover from the pre-Phase-3 Applet days -- it has
    // no visible content of its own (just a bare AWT Panel) and never did;
    // the real UI (TabbedWindow, plus a new JFrame per <window> XML tag) is
    // created independently by ClientManager, which ClientPanel.start()
    // constructs. Wrapping it in its own JFrame ("Test", renamed from
    // nothing before this) was never doing anything but hosting the one
    // real EXIT_ON_CLOSE in the whole app -- that's now ClientManager's own
    // TabbedWindow's job instead, so this doesn't need a JFrame at all.
    private static void createAndShowGUI(String server, String uname, String password) {
        ClientPanel applet = new ClientPanel();
        applet.init(server,uname,password);
        applet.start();
    }
}
