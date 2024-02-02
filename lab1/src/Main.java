import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.HashMap;
import java.util.Map;
import java.util.LinkedHashMap;
import java.util.stream.Collectors;
import java.io.BufferedWriter;
import java.io.FileWriter;


public class Main {
    public static void main(String[] args) {
        int totalFrequency = 0;
        String fileName = args[0];
        Reader reader = null;
        try {
            reader = new InputStreamReader(new FileInputStream(fileName + ".txt"));
            StringBuilder currentWordBuilder = new StringBuilder();
            int currentChar;
            Map<String, Integer> wordCounterMap = new HashMap<>();

            while ((currentChar = reader.read()) != -1) {
                char currentCharacter = (char) currentChar;

                if (Character.isLetterOrDigit(currentCharacter)) {
                    currentWordBuilder.append(currentCharacter);
                } else {
                    String currentWord = currentWordBuilder.toString();
                    wordCounterMap.merge(currentWord, 1, Integer::sum);
                    currentWordBuilder.setLength(0); // Очищаем StringBuilder для следующего слова
                    ++totalFrequency;
                }
            }

            Map<String, Integer> sortedMap = wordCounterMap.entrySet()
                    .stream()
                    .sorted(Map.Entry.<String, Integer>comparingByValue().reversed())
                    .collect(Collectors.toMap(
                            Map.Entry::getKey,
                            Map.Entry::getValue,
                            (e1, e2) -> e1,
                            LinkedHashMap::new
                    ));

            String csvFilePath = "output.csv";
            try (BufferedWriter writer = new BufferedWriter(new FileWriter(csvFilePath))) {
                writer.write("Word,Frequency,Frequency(%)\n");

                for (Map.Entry<String, Integer> entry : sortedMap.entrySet()) {
                    writer.write(entry.getKey() + "," + entry.getValue() + "," + String.format("%.0f", ((double) entry.getValue() / totalFrequency) * 100.0) + "\n");
                }
                System.out.println("The data has been successfully written to a CSV file.");

            } catch (IOException e) {
                e.printStackTrace();
            }


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
