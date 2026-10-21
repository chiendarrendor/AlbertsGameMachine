import java.net.URI;
import java.net.URL;
import java.net.URLClassLoader;
import java.lang.reflect.Method;

// Minimal bootstrap: the only thing ever run/downloaded locally. Fetches
// Client.jar live from CloudFront on every launch (same mechanism
// GUIUnit.java already uses for per-game jars, driven by the server-sent
// XMLLoc -- this deliberately doesn't second-guess that, it only ever
// needs to classload Client.jar) and hands off to the real entry point.
public class Launcher
{
  public static void main(String[] args) throws Exception
  {
    URL[] urls = { URI.create("https://legacy.albertsgamemachine.net/Client.jar").toURL() };
    URLClassLoader loader = new URLClassLoader(urls);
    Class<?> app = Class.forName("TheGameMachine.ClientApplication",true,loader);
    app.getMethod("main",String[].class).invoke(null,(Object)args);
  }
}
