package OpenZorz;

import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.Enumeration;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import javax.swing.ImageIcon;

public class TestResource {

    public static ImageIcon getImageIconFromJar(String jarURL,String imageName)  {
        try {
            URL[] loaderurls = new URL[]{new URL(jarURL)};
            ClassLoader cl = new URLClassLoader(loaderurls);

            Enumeration<URL> imageurls = cl.getResources(imageName);
            while (imageurls.hasMoreElements()) {
                URL url = imageurls.nextElement();
                ImageIcon icon = new ImageIcon(url);
                return icon;
            }
            return null;
        } catch (MalformedURLException mue) {
            throw new RuntimeException("getImageIconFromJar", mue);
        } catch (IOException e) {
            throw new RuntimeException("getImageIconFromJar", e);
        }


    }


    public static void main(String[] args) {
        System.out.println("Worked:");
        ImageIcon icon = getImageIconFromJar(args[0],"Man.PNG");
        System.out.println("W,H: " + icon.getIconWidth() + "," + icon.getIconHeight());
        System.out.println("Trials:");

        try {
            URL[] urls = new URL[]{new URL(args[0]) };
            ClassLoader cl = new URLClassLoader(urls);

            String target = "Man.PNG";
            System.out.println ("--- " + target + " ---");
            Enumeration<URL> metaurls = cl.getResources(target);
            if (metaurls.hasMoreElements()) {
                URL url = metaurls.nextElement();

                ImageIcon ii = new ImageIcon(url);
                System.out.println("W,H: " + ii.getIconWidth() + "," + ii.getIconHeight());

            }
            System.out.println("--- done with " + target + " ---");

            URL imageURL = cl.getResource("/Man.PNG");
            if (imageURL == null) {
                System.out.println("imageURL is null");
            } else {
                ImageIcon ii = new ImageIcon(imageURL);
            }

            InputStream is = cl.getResourceAsStream("/Man.JPG");
            if (is == null) {
                System.out.println("Image Stream is null");
            } else {
                byte[] imageBytes = new byte[is.available()];
                is.read(imageBytes);
                ImageIcon ii2 = new ImageIcon(imageBytes);
            }


            Class fc = cl.loadClass("Outpost.PlayerWindow");
            if (fc == null) {
                System.out.println("Class Load is null");
            } else {
                System.out.println("Class: " + fc);
            }


        } catch (MalformedURLException e) {
            throw new RuntimeException(e);
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
