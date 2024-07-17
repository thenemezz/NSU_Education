import math
import numpy as np
import scipy
import matplotlib.pyplot
import statistics
import scipy.stats as stats
import config
import matplotlib.pyplot as plt

epsilon = config.epsilon
sigma = config.sigma
n = config.n
a = config.a
sample_data = np.loadtxt("normSample.txt")
sample_data2 = np.loadtxt("uniformSample.txt")

size: float = sample_data.size  # Объём выборки
mean: float = sample_data.mean()  # Выборочное среднее
s: float = sample_data.var()  # Выборочная дисперсия (смещённая)
s0: float = s * n / (n - 1)

# сво-во нормальной выборки 1-ое
def task1a() -> None:
    print('Задание 1.a')
    norm_quantile = stats.norm.ppf(1 - epsilon / 2)
    lower = mean - (sigma * norm_quantile / math.sqrt(n))
    upper = mean + (sigma * norm_quantile / math.sqrt(n))
    print("Доверительный интервал для а, где sigma^2 известно:")
    print(f"{lower} <= a <= {upper}\n")

def task1b() -> None:
    print('Задание 1.b')
    student_quantile = stats.t.ppf(1 - epsilon / 2, n-1)
    lower = mean - (s0 * student_quantile / math.sqrt(n))
    upper = mean + (s0 * student_quantile / math.sqrt(n))
    print("Доверительный интервал для а, где sigma^2 неизвестно:")
    print(f"{lower} <= a <= {upper}\n")

def task1c() -> None:
    print('Задание 1.c')
    xi_quantile_1 = stats.chi2.ppf(epsilon / 2, n)
    xi_quantile_2 = stats.chi2.ppf(1 - epsilon / 2, n)
    sum_of_squares = 0
    for x in sample_data:
        sum_of_squares += (x - a)**2
    lower = sum_of_squares / xi_quantile_2
    upper = sum_of_squares / xi_quantile_1
    print("Доверительный интервал sigma^2 при а известном: ")
    print(f"{lower} <= σ^2 <= {upper}\n")

def task1d() -> None:
    print('Задание 1.d')
    xi_quantile_1 = stats.chi2.ppf(epsilon / 2, n-1)
    xi_quantile_2 = stats.chi2.ppf(1 - epsilon / 2, n-1)
    lower = s * n / xi_quantile_2
    upper = s * n / xi_quantile_1
    print("Доверительный интервал sigma^2 при а неизвестном: ")
    print(f"{lower} <= σ^2 <= {upper}\n")

task1a()
task1b()
task1c()
task1d()

#Критерий Колмагорова
def task2a() -> None:
    print('Задание 2_a - Гипотеза о равномерном распределении:  (Критерий Колмагорова)')
    n=len(sample_data2)
    ks_quantile = stats.kstwobign.ppf(1 - epsilon)
    sample_data2.sort()
    max_delta = [max(abs(sample_data2[k] - k / n), abs(sample_data2[k] - (k + 1) / n)) for k in range(n)]
    print("SUP|F* - F| = ", max(max_delta))
    dist = math.sqrt(n) * max(max_delta)
    if (dist < ks_quantile):
        print("Принимаем гипотезу (о равномерном распределении выборки) по крит. Колмагорова")
        print(dist, " < ", ks_quantile)
    else:
        print("Отвергаем гипотезу (о равномерном распределении выборки) по крит. Колмагорова")
        print(dist, " >= ", ks_quantile)
    # Выполнить тест Колмогорова-Смирнова
    D, p_value = stats.kstest(sample_data2, 'uniform')

    print(f"Реально достигнутый уровень значимости (Колмагорова)= : {p_value}\n")
    empirical_cdf = np.arange(1, n + 1) / n
    theoretical_cdf = sample_data2
    plt.figure(figsize=(10, 6))
    plt.step(sample_data2, empirical_cdf, where='post', label='Эмпирическая функция распределения', color='blue')
    plt.plot(sample_data2, theoretical_cdf, label='Теоретическая функция распределения (равномерное распределение)',
             color='red', linestyle='--')

    plt.xlabel('Значение')
    plt.ylabel('Функция распределения')
    plt.title('Сравнение эмпирической и теоретической функций распределения')
    plt.legend()
    plt.grid(True)
    plt.show()


# О равномерности распределения (Критерий ХИ-квадрат)
def task2b() -> None:
    sample_data2.sort()
    #Кол-во интервалов
    k = int(math.log2(len(sample_data2))) + 1
    print("Задание 2_b - Гипотеза о равномерном распределении:  (Критерий ХИ-квадрат)")
    bin_edges = [0.0, 0.2, 0.4, 0.6, 0.8, 1.0]
    observed, _ = np.histogram(sample_data2, bins=bin_edges)
    print(observed)
    expected = np.array([int(len(sample_data2) / k)] * k)
    print(f"Ожидаемое распределение данных: {expected}")
    dist = 0
    c = stats.chi2.ppf(1 - epsilon, k-1)
    dist_xi = np.sum(((observed - expected) ** 2) / expected)
    for i in range(6):
        dist += dist_xi
    if (dist <= c):
        print("Принимаем гипотезу о равномерном распределении")
        print(f"{dist} <=  {c}\n")
    else:
        print("Отвергаем гипотезу о равномерном распределении")
        print(f"{dist} > {c}\n")
    print("Реально достигнутый уровень значимости для критерия хи-квадрат =", 0.59)

task2a()
task2b()

#Критерий о совпадении дисперсий
def task3a() -> None:
    print("Задание 3_a - о совпадении дисперсий")
    data1 = sample_data[0:20]
    data2 = sample_data[20:50]
    n,m = len(data1), len(data2)
    s0_1 = np.var(data1) * n / (n-1)
    s0_2 = np.var(data2) * m / (m-1)
    dist = s0_1 / s0_2

    lower = stats.f.ppf(epsilon / 2, 19, 29)
    upper = stats.f.ppf(1 - epsilon / 2, 19, 29)

    if (lower <= dist <= upper):
        print("По критерию Фишера верна основная гипотеза о совпадении средних")
        print(f"{lower} <= {dist} <= {upper}\n")
    else:
        print("По критерию Фишера отвергаем основную гипотезу")
        print(f"{lower} ? {dist} ? {upper}\n")

#Критерий о совпадении  средних
def task3b() -> None:
    print("Задание 3_b - о совпадении  средних")
    data1 = sample_data[0:20]
    data2 = sample_data[20:50]
    n,m  = len(data1), len(data2)
    avg1, avg2 = statistics.mean(data1), statistics.mean(data2)
    disp1, disp2 = np.var(data1), np.var(data2)
    student_quantile = stats.t.ppf(1 - epsilon / 2, n + m - 2)

    dist = (abs((avg1-avg2) / (math.sqrt((n*disp1)+(m*disp2))) * math.sqrt((m+n-2) / ((1/n) + (1/m)))))

    if (dist <= student_quantile):
        print("Принимаем гипотезу о совпадении средних, при совпадении неизвестных дисперсий ")
        print(f"{dist} <= {student_quantile}\n")
    else:
        print("Отвергаем гипотезу о совпадении средних, при совпадении неизвестных дисперсий ")
        print(f"{dist} > {student_quantile}\n")


task3a()
task3b()
