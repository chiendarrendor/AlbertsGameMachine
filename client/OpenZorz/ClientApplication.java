package OpenZorz;

import javax.swing.SwingUtilities;
import javax.swing.JFrame;


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



    private static void createAndShowGUI(String server, String uname, String password) {
        System.out.println("Created GUI on EDT? "+
                SwingUtilities.isEventDispatchThread());
        JFrame f = new JFrame("Test");
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        ClientApplet applet = new ClientApplet();
        applet.init(server,uname,password);
        f.getContentPane().add(applet);

        f.pack();
        f.setVisible(true);
        applet.start();

    }
}
