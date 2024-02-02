import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;

public class stringBuilder {
    public static void main(String[] args) {
        String tmpStr = args[0];
        Reader reader = null;
        try
        {
            reader = new InputStreamReader(new FileInputStream(tmpStr + ".txt"));
            //read the data here
        }
        catch (IOException e)
        {
            System.err.println("Error while reading file: " + e.getLocalizedMessage());
        }
        finally
        {
            if (null != reader)
            {
                try
                {
                    reader.close();
                }
                catch (IOException e)
                {
                    e.printStackTrace(System.err);
                }
            }
        }

    }
}
