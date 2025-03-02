def user_input():
    while True:
        try:
            print("Введите коэффициенты кубического уравнения через пробел (a, b, c, d):")
            values = input().split()
            if len(values) != 4:
                print("Ошибка: необходимо ввести ровно 4 числа")
                continue
            a, b, c, d = map(float, values)
            return a, b, c, d
        except ValueError:
            print("Ошибка: ввод должен содержать только числа. Попробуйте снова")


def f(x, a, b, c, d):
    return a * x ** 3 + b * x ** 2 + c * x + d


def df(x, a, b, c):
    return 3 * a * x ** 2 + 2 * b * x + c


def d2f(x, a, b):
    return 6 * a * x + 2 * b


def chord(a, b, c, d, st, end):
    """Метод хорд"""
    return st - f(st, a, b, c, d) / (f(end, a, b, c, d) - f(st, a, b, c, d)) * (end - st)


def tangent(a, b, c, d, st):
    """Метод касательных"""
    return st - f(st, a, b, c, d) / df(st, a, b, c)


def valid_df(x: float, a: float, b: float, c: float) -> bool:
    """Проверяет, является ли производная df(x) достаточно большой, чтобы избежать деления на слишком малые значения"""
    return abs(df(x, a, b, c)) > 1e-15


def get_precision() -> float:
    """Запрашивает у пользователя точность e для численного метода (e ∈ (0, 1))"""
    while True:
        try:
            e = float(input("Введите точность вычислений (например, 0.1): "))
            if not (0 < e < 1):
                print("Ошибка: точность должна быть числом в интервале (0, 1)")
                continue
            return e
        except ValueError:
            print("Ошибка: введите корректное число")


def round_to_precision(value: float, precision: float) -> float:
    """Округляет число до значащих цифр, соответствующих точности e"""
    return round(value, abs(int(f"{precision:e}".split("e")[-1])))


def combined_method(start, end, e, a, b, c, d):
    """Комбинированный метод хорд и касательных для кубического уравнения"""
    while abs(start - end) > e:
        if f(start, a, b, c, d) * d2f(start, a, b) <= 0 and valid_df(end, a, b, c):
            start = chord(a, b, c, d, start, end)
        else:
            start = tangent(a, b, c, d, start)
        
        if f(end, a, b, c, d) * d2f(end, a, b) <= 0 and valid_df(start, a, b, c):
            end = chord(a, b, c, d, end, start)
        else:
            end = tangent(a, b, c, d, end)
    return round_to_precision((start + end) / 2, e)


def non_cubic_solver(e, b, c, d):
    if b == 0:
        if c == 0:
            if d == 0:
                print("Решение: любое число")
            else:
                print("Решений нет")
        else:
            print(f"Решение: x = {round_to_precision(-d / c, e)}")
    else:
        disc = c ** 2 - 4 * b * d
        if disc < 0:
            print("Вещественных решений нет")
        elif disc == 0:
            print(f"Решение: x = {round_to_precision(-c / (2 * b), e)}")
        else:
            sqrt_disc = disc ** 0.5
            x1 = (-c + sqrt_disc) / (2 * b)
            x2 = (-c - sqrt_disc) / (2 * b)

            if x1 == x2:
                print(f"Решение: x = {round_to_precision(x1, e)}")
            else:
                print(f"Решение: x1 = {round_to_precision(x1, e)}, x2 = {round_to_precision(x2, e)}")


def cubic_solver(e, a, b, c, d):
    mmax = 1 + max(abs(a), abs(b), abs(c), abs(d)) / abs(a)
    mmin = -mmax
    infl_point = -b / (3 * a)
    disc = 4 * b ** 2 - 12 * a * c

    if disc <= 0:
        if f(infl_point, a, b, c, d) * a > 0:
            x = combined_method(mmin, infl_point, e, a, b, c, d)
        elif f(infl_point, a, b, c, d) * a < 0:
            x = combined_method(infl_point, mmax, e, a, b, c, d)
        else:
            x = infl_point
        print(f"Решение: x = {round_to_precision(x, e)}")
    else:
        # поиск экстремумов
        ex1 = (-2 * b - disc ** 0.5) / (6 * a)
        ex2 = (-2 * b + disc ** 0.5) / (6 * a)
        min_point = min(ex1, ex2)
        max_point = max(ex1, ex2)

        if f(min_point, a, b, c, d) * f(max_point, a, b, c, d) > 0:
            if f(infl_point, a, b, c, d) * a > 0:
                x = combined_method(mmin, min_point, e, a, b, c, d)
            else:
                x = combined_method(max_point, mmax, e, a, b, c, d)
            print(f"Решение: x = {round_to_precision(x, e)}")

        elif f(min_point, a, b, c, d) * f(max_point, a, b, c, d) == 0:
            x2 = max_point
            if f(infl_point, a, b, c, d) * a > 0:
                x1 = combined_method(mmin, min_point, e, a, b, c, d)
            else:
                x1 = combined_method(max_point, mmax, e, a, b, c, d)
            print(f"Решение: x1 = {round_to_precision(x1, e)}, x2 = {round_to_precision(x2, e)}")

        else:
            x1 = combined_method(mmin, min_point, e, a, b, c, d)
            x3 = combined_method(max_point, mmax, e, a, b, c, d)
            if f(infl_point, a, b, c, d) * a == 0:
                x2 = infl_point
            elif f(infl_point, a, b, c, d) * a > 0:
                x2 = combined_method(infl_point, max_point, e, a, b, c, d)
            else:
                x2 = combined_method(min_point, infl_point, e, a, b, c, d)
            print(f"Решение: x1 = {round_to_precision(x1, e)}, x2 = {round_to_precision(x2, e)}, x3 = {round_to_precision(x3, e)}")


def main():
    a, b, c, d = user_input()

    # запрашиваем точность
    e = get_precision()

    if a == 0:
        non_cubic_solver(e, b, c, d)
    else:
        cubic_solver(e, a, b, c, d)

if __name__ == "__main__":
    main()
