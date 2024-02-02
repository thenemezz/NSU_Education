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
            String currentWord = "";
            int currentChar;
            Map<String, Integer> wordCounterMap = new HashMap<>();

            while ((currentChar = reader.read()) != -1) {
                if (Character.isLetterOrDigit((char) currentChar)) {
                     currentWord += (char) currentChar;
                }
                else {
                    wordCounterMap.merge(currentWord, 1, Integer::sum);
                    currentWord = "";
                    ++totalFrequency;
                }
            }
            // хз как красИвей отсортировать мапу
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

                // Записываем данные из хеш-таблицы
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

//сортировка мапы другая
//            wordCounterMap.entrySet().stream()
//                    .sorted(Map.Entry.<String, Integer>comparingByValue().reversed())
//                    .forEach(System.out::println);

//вывод отсортированной мапы
//            for (Map.Entry<String, Integer> entry : sortedMap.entrySet()) {
//                System.out.println(entry.getKey() + " (частота: " + entry.getValue() + ")");
//            }