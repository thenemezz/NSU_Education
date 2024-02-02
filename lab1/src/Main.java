import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
//import stringBuilder.java
import java.util.LinkedHashMap;
import java.util.stream.Collectors;
import java.io.BufferedWriter;
import java.io.FileWriter;


public class Main {
    public static void main(String[] args) {
        int totalFrequency = 0;
        String tmpStr = args[0];
        Reader reader = null;
        try {
            reader = new InputStreamReader(new FileInputStream(tmpStr + ".txt"));
            String tmp_str = "";
            int currentChar;
            Map<String, Integer> wordCounterMap = new HashMap<>();
            while ((currentChar = reader.read()) != -1) {

                char letter = (char) currentChar;

                if (Character.isLetterOrDigit(letter)) {
                     tmp_str = tmp_str + letter;
                }
                else {
                    wordCounterMap.merge(tmp_str, 1, Integer::sum);
                    tmp_str = "";
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
            String csvFilePath = "test.csv";
            try (BufferedWriter writer = new BufferedWriter(new FileWriter(csvFilePath))) {
                writer.write("Word,frequency,frequency(%)\n");

                // Записываем данные из хеш-таблицы
                for (Map.Entry<String, Integer> entry : sortedMap.entrySet()) {
                    double percentage = ((double) entry.getValue() / totalFrequency) * 100.0;
                    //String formattedPercentage = String.format("%.2f", percentage);
                    writer.write(entry.getKey() + "," + entry.getValue() + "," + String.format("%.0f", percentage) + "\n");
                }

                //writer.write(entry.getKey() + "," + entry.getValue() + "," + + ""\n");
                System.out.println("Данные успешно записаны в CSV файл.");
            } catch (IOException e) {
                e.printStackTrace();
            }
//            wordCounterMap.entrySet().stream()
//                    .sorted(Map.Entry.<String, Integer>comparingByValue().reversed())
//                    .forEach(System.out::println);


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

//вывод отсортированной мапы
//            for (Map.Entry<String, Integer> entry : sortedMap.entrySet()) {
//                System.out.println(entry.getKey() + " (частота: " + entry.getValue() + ")");
//            }