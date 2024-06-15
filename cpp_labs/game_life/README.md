## Игра “Жизнь”
Игра «Жизнь» - клеточный автомат, подробно описанный по ссылке http://en.wikipedia.org/wiki/Conway's_Game_of_Life

Требования к программе:
Уметь считывать «вселенную» из файла (см. формат файла ниже). Вселенная описывается (а) правилом перехода клетки; (б) размером поля; (в) состоянием ячеек. Файл может подаваться программе при старте в качестве параметра входной строки.

В случае, если файл не подан, начинать игру на произвольно инициализированном поле некоего фиксированного размера, красиво отображающегося на консоли. Поле, инициализированное случайным образом малоинтересно, поэтому рекомендуется начинать игру на каких-нибудь заранее продуманных примерах.

Третий режим ("оффлайн"): программе подается входной файл, количество итераций, выходной файл. Программа рассчитывает состояние поля после указанного количества итераций и сохраняет вселенную в указанный выходной файл. Параметры командной строки: -i x либо --iterations=x - количество итераций, -o filename либо --output=filename - выходной файл.

После старта игры, программа ожидает команды:
dump <filename> - сохранить вселенную в файл
tick <n=1> (и сокращенно t <n=1>) - рассчитать n (по умолчанию 1) итераций и напечатать результат. На экране перед распечатанным полем должно отображаться имя вселенной, правило перехода и номер текущей итерации.
exit – завершить игру
help – распечатать справку о командах