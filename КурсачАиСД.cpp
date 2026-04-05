#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>
#include <random>
#include <cstring>
#include <string>
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace chrono;

class Matrix
{
private:
    long long int** data;
    size_t num_of_rows; //размер стороны матрицы
    size_t num_of_columns;
    long long int mod; //module

    long long int modOP(long long int value) const 
    {
        return (value % mod + mod) % mod;
    }

public:
    //Конструктор по умолчанию
    Matrix() : data(nullptr), num_of_rows(0), num_of_columns(0), mod(0) {}

    // Конструктор с заданными размерами матрицы и модулем
    Matrix(size_t num_rows, size_t num_columns, long long int module) : num_of_rows(num_rows), num_of_columns(num_columns), mod(module)
    {
        if ((num_of_rows <= 0) || (num_of_columns <= 0))
        {
            data = nullptr;
            num_of_rows = 0;
            num_of_columns = 0;
            return;
        }

        if (mod < 2 || mod >= LLONG_MAX) 
        {
            mod = 0;
            throw invalid_argument("Module must be in the range [2, LLONG_MAX).");
        }
         
        data = new(nothrow) long long int* [num_of_rows];
        if (!data) throw "Error: Memory allocation failed.";

        for (size_t i = 0; i < num_of_rows; ++i)
        {
            data[i] = new(nothrow) long long int[num_of_columns]();
            if (!data[i]) throw "Error: Memory allocation failed.";
        }
    }

    // Конструктор копирования
    Matrix(const Matrix& other) : num_of_rows(other.num_of_rows), num_of_columns(other.num_of_columns), mod(other.mod)
    {
        data = new(nothrow) long long int* [num_of_rows];
        if (!data) throw "Error: Memory allocation failed.";

        for (size_t i = 0; i < num_of_rows; ++i)
        {
            data[i] = new(nothrow) long long int[num_of_columns];
            if (!data[i]) throw "Error: Memory allocation failed.";

            for (size_t j = 0; j < num_of_columns; ++j)
            {
                data[i][j] = other.data[i][j];
            }
        }
    }

    // Деструктор
    ~Matrix()
    {
        for (size_t i = 0; i < num_of_rows; ++i)
        {
            delete[] data[i];
        }
        delete[] data;

        num_of_rows = 0;
        num_of_columns = 0;
        mod = 0;
    }

    // Метод для установки значения элемента
    void setElement(size_t row, size_t col, long long int value)
    {
        if (row >= num_of_rows || col >= num_of_columns) 
        {
            throw out_of_range("Error: Index out of bounds");
        }
        data[row][col] = modOP(value);
    }

    // Операция сложения по модулю
    Matrix* add(const Matrix* other) const 
    {
        if ((num_of_rows != other->num_of_rows) || (num_of_columns != other->num_of_columns)) 
        {
            throw "Matrix sizes do not match for addition.";
        }

        Matrix* result = new Matrix(num_of_rows, num_of_columns, mod);

        for (size_t i = 0; i < num_of_rows; ++i) 
        {
            for (size_t j = 0; j < num_of_columns; ++j) 
            {
                result->data[i][j] = modOP(data[i][j] + other->data[i][j]);
            }
        }
        return result;
    }

    //Операция вычитания матриц по модулю
    Matrix* subtract(const Matrix* other) const 
    {
        if ((num_of_rows != other->num_of_rows) || (num_of_columns != other->num_of_columns)) 
        {
            throw "Matrix sizes do not match for subtraction.";
        }

        Matrix* result = new Matrix(num_of_rows, num_of_columns, mod);
        for (size_t i = 0; i < num_of_rows; ++i) 
        {
            for (size_t j = 0; j < num_of_columns; ++j) 
            {
                result->data[i][j] = modOP(data[i][j] - other->data[i][j]);
            }
        }
        return result;
    }

    //Операция умножения матриц по модулю
    Matrix* multiply(const Matrix* other) const 
    {
        if (num_of_columns != other->num_of_rows) 
        {
            throw "Matrix sizes do not match for multiplication.";
        }

        Matrix* result = new Matrix(num_of_rows, other->num_of_columns, mod);
        for (size_t i = 0; i < num_of_rows; ++i) 
        {
            for (size_t j = 0; j < other->num_of_columns; ++j) 
            {
                long long int sum = 0;
                for (size_t k = 0; k < num_of_columns; ++k) 
                {
                    sum = modOP(sum + modOP(data[i][k] * other->data[k][j]));
                }
                result->data[i][j] = sum;
            }
        }
        return result;
    }

    //ЭП: перестановка двух строк местами
    void swapRows(size_t row1, size_t row2) 
    {
        if (row1 >= num_of_rows || row2 >= num_of_rows) 
        {
            throw out_of_range("Error: Row index out of bounds.");
        }

        for (size_t col = 0; col < num_of_columns; ++col) 
        {
            long long int temp = data[row1][col];
            data[row1][col] = data[row2][col];
            data[row2][col] = temp;
        }
    }

    // Умножение всех элементов строки на ненулевое число по модулю
    void multiplyRow(size_t row, long long int factor) 
    {
        if (row >= num_of_rows) 
        {
            throw out_of_range("Error: Row index out of bounds.");
        }
        if (factor == 0) 
        {
            throw invalid_argument("Error: Factor cannot be zero.");
        }

        factor = modOP(factor); // Приводим множитель к кольцу вычетов
        for (size_t col = 0; col < num_of_columns; ++col) 
        {
            data[row][col] = modOP(data[row][col] * factor);
        }
    }

    // Сложение строки с другой строкой, умноженной на ненулевое число по модулю
    void addRowMultiple(size_t targetRow, size_t sourceRow, long long int factor) 
    {
        if (targetRow >= num_of_rows || sourceRow >= num_of_rows) 
        {
            throw out_of_range("Error: Row index out of bounds.");
        }
        if (factor == 0) 
        {
            throw invalid_argument("Error: Factor cannot be zero.");
        }

        factor = modOP(factor); // Приводим множитель к кольцу вычетов
        for (size_t col = 0; col < num_of_columns; ++col) 
        {
            data[targetRow][col] = modOP(data[targetRow][col] + modOP(data[sourceRow][col] * factor));
        }
    }

    //Выделение подматрицы путём удаления строки и/или столбца
    Matrix* subMatrix(bool excludeRowFlag, size_t excludeRow, bool excludeColFlag, size_t excludeCol) const //если удалять не нужно, передаём индекс равен 
    {
        // Проверяем, нужно ли что-то удалять
        size_t newRows = excludeRowFlag ? num_of_rows - 1 : num_of_rows;
        size_t newCols = excludeColFlag ? num_of_columns - 1 : num_of_columns;

        if ((excludeRowFlag && excludeRow >= num_of_rows) ||
            (excludeColFlag && excludeCol >= num_of_columns))
        {
            throw out_of_range("Error: Index out of bounds for subMatrix.");
        }

        // Создаем новую матрицу с модулем текущей матрицы
        Matrix* result = new Matrix(newRows, newCols, mod);

        size_t newRow = 0; // Индекс для новой матрицы
        for (size_t i = 0; i < num_of_rows; ++i)
        {
            if (excludeRowFlag && i == excludeRow) continue; // Пропускаем строку
            size_t newCol = 0;
            for (size_t j = 0; j < num_of_columns; ++j)
            {
                if (excludeColFlag && j == excludeCol) continue; // Пропускаем столбец
                result->setElement(newRow, newCol, data[i][j]);
                ++newCol;
            }
            ++newRow;
        }
        return result;
    }

    // Преобразование матрицы в строку
    string toString() const 
    {
        ostringstream result;
        for (size_t i = 0; i < num_of_rows; ++i) 
        {
            for (size_t j = 0; j < num_of_columns; ++j) 
            {
                result << data[i][j];
                if (j < num_of_columns - 1) 
                {
                    result << "\t"; // Используем табуляцию
                }
            }
            if (i < num_of_rows - 1) 
            {
                result << "\n";
            }
        }
        return result.str();
    }

    // Преобразование строки в матрицу
    void fromString(const string matrixString, size_t num_rows, size_t num_columns, long long int module) 
    {
        istringstream input(matrixString);

        long long int value;
        for (size_t i = 0; i < num_rows; ++i) 
        {
            for (size_t j = 0; j < num_columns; ++j) 
            {
                if (!(input >> value)) {
                    throw invalid_argument("Error: Input string does not match the specified matrix dimensions.");
                }
                this->setElement(i, j, modOP(value));
            }
        }

        // Проверяем, что в строке не осталось лишних данных
        long long int extraValue;
        if (input >> extraValue) 
        {
            throw invalid_argument("Error: Input string contains extra values beyond the specified matrix dimensions.");
        }
    }

    //Вывод матрицы на экран
    void printMatrix() 
    {
        cout << '\n';
        cout << toString() << endl;
        cout << '\n';
    }

    //поиск обратного элемента по модулю 
    long long int modularInverse(long long int value) const 
    {
        //value = modOP(value); // Приводим значение к кольцу вычетов
        for (long long int i = 1; i < mod; ++i) 
        {
            if ((value * i) % mod == 1) 
            {
                return i;
            }
        }
        return -1;
    }

    //Нормировка строки - Последний шаг обратного хода метода Гаусса (умножение на обратный по модулю элемент)
    void normalizeRow(size_t row) 
    {
        long long int leadingElement = data[row][row];
        long long int inverse = modularInverse(leadingElement);
        multiplyRow(row, inverse); // Умножаем строку на обратный элемент
    }
    
    //Формирование бесконечного решения СЛУ
    string* generateFundamentalSystem(size_t rank, size_t* columnMap) const
    {
        string* result = new string[num_of_columns - 1];
        for (size_t i = num_of_columns - 2; i != rank - 1; i--)
        {
            result[i] = 'x' + to_string(columnMap[i] + 1) + " = " + "a" + to_string(num_of_columns - 1 - i);
        }

        for (size_t i = num_of_rows; i > 0; i--)
        {
            result[i - 1] = "x" + to_string(columnMap[i - 1] + 1) + " = " + to_string(data[i - 1][num_of_columns - 1]) + " + ";
            for (size_t j = 0; j < (num_of_columns - 1) - rank; j++)
            {
                result[i - 1] += to_string(modOP((-1) * data[i - 1][num_of_columns - 2 - j])) + "a" + to_string(j + 1) + ' ';
                if (j != num_of_columns - rank - 2)  result[i - 1] += " + ";
            }
        }
        return result;
    }

    //Прямой ход метода Гаусса
    void gaussEliminationForward() 
    {
        size_t row = 0;
        for (size_t col = 0; col < num_of_columns - 1; ++col) 
        {
            // Найдем строку с ненулевым элементом в текущем столбце
            size_t nonZeroRow = row;
            while (nonZeroRow < num_of_rows && data[nonZeroRow][col] == 0) 
            {
                ++nonZeroRow;
            }

            // Если строка с ненулевым элементом не найдена, пропускаем этот столбец
            if (nonZeroRow == num_of_rows) 
            {
                continue;
            }

            // Переставляем строки, чтобы текущий столбец имел ненулевой элемент
            if (nonZeroRow != row) 
            {
                swapRows(row, nonZeroRow);
            }

            // Преобразуем элементы в текущем столбце ниже главного элемента в нули
            for (size_t i = row + 1; i < num_of_rows; ++i) 
            {
                if (data[i][col] != 0) 
                {
                    long long int factor = (-1) * (data[i][col]);
                    multiplyRow(i, data[row][col]);
                    addRowMultiple(i, row, factor); 
                }
            }

            // Переходим к следующей строке
            ++row;
        }
    }
    
    //Вычисление ранга матрицы
    size_t calculateRank(size_t num_of_rows, size_t num_of_columns)
    {
        size_t rank = 0;
        for (size_t i = 0; i < num_of_rows; ++i) 
        {
            bool isZeroRow = true;
            for (size_t j = 0; j < num_of_columns; ++j) 
            {
                if (data[i][j] != 0) 
                {
                    isZeroRow = false;
                    break;
                }
            }
            if (!isZeroRow) 
            {
                ++rank;
            }
        }
        return rank;
    }

    //Удаление нулевых строк
    void removeZeroRows() 
    {
        size_t newRowCount = 0;
        for (size_t i = 0; i < num_of_rows; ++i) 
        {
            bool isZeroRow = true;
            for (size_t j = 0; j < num_of_columns - 1; ++j) 
            {
                if (data[i][j] != 0) 
                {
                    isZeroRow = false;
                    break;
                }
            }

            if (!isZeroRow) 
            {
                if (i != newRowCount) 
                {
                    swapRows(i, newRowCount);
                }
                ++newRowCount;
            }
        }

        num_of_rows = newRowCount;
    }

    //Перемещаем нулевые столбцы вправо
    void moveZeroColumnsRight(size_t* columnMap) 
    {
        size_t numColumns = num_of_columns - 1; // Учитываем, что последний столбец — столбец свободных членов
        size_t* newColumnOrder = new size_t[numColumns];
        for (size_t i = 0; i < numColumns; ++i) 
        {
            newColumnOrder[i] = i;
        }

        bool moved = false; // Флаг для отслеживания перемещений

        // Проходим по всем столбцам, кроме последнего
        size_t lastNonZeroCol = 0;
        for (size_t col = 0; col < numColumns; ++col) 
        {
            bool isZeroColumn = true;
            for (size_t row = 0; row < num_of_rows; ++row) 
            {
                if (data[row][col] != 0) 
                {
                    isZeroColumn = false;
                    break;
                }
            }

            if (!isZeroColumn) 
            {
                // Если столбец не нулевой, оставляем его на текущей позиции
                if (lastNonZeroCol != col) 
                {
                    // Меняем местами столбцы
                    for (size_t row = 0; row < num_of_rows; ++row) 
                    {
                        swap(data[row][lastNonZeroCol], data[row][col]);
                    }
                    swap(newColumnOrder[lastNonZeroCol], newColumnOrder[col]);
                    //newColumnOrder[lastNonZeroCol] = col;
                    moved = true; // Зафиксировали перемещение
                }
                ++lastNonZeroCol;
            }
        }

        // Сохраняем порядок столбцов
        for (size_t i = 0; i < numColumns; ++i) 
        {
            columnMap[i] = newColumnOrder[i];
        }

        delete[] newColumnOrder;
    }

    //Обратный ход метода Гаусса
    void gaussEliminationBackward() 
    {
        for (size_t row = num_of_rows; row > 0; --row) 
        {
            size_t col = 0;
            while (col < num_of_columns - 1 && data[row - 1][col] == 0) 
            {
                ++col;
            }
            if (col == num_of_columns - 1) 
            {
                continue;
            }

            if (num_of_rows != 1)
            {
                // Преобразуем элементы выше ведущего элемента в нули
                for (size_t i = row - 1; i > 0; --i)
                {
                    if (data[i - 1][col] != 0) 
                    {
                        long long int factor = (-1) * (data[i - 1][col]);
                        multiplyRow(i - 1, data[row - 1][col]);
                        addRowMultiple(i - 1, row - 1, factor);
                    }
                }
            }
        }

        for (size_t i = 0; i < num_of_rows; i++)
        {
            normalizeRow(i);
        }
    }

    //Решение СЛУ
    string* solve()
    {
        // Прямой ход
        gaussEliminationForward();
        printMatrix();
        cout << '\n';

        // Проверка ранга и удаление нулевых строк
        size_t rank = calculateRank(num_of_rows, num_of_columns - 1); // Ранг матрицы коэффициентов
        size_t augmentedRank = calculateRank(num_of_rows, num_of_columns); // Ранг расширенной матрицы 

        removeZeroRows();

        // Если ранг меньше числа переменных, система не имеет решения или имеет бесконечно много решений
        if (rank < augmentedRank)
        {
            string* result = new string[1];
            result[0] = "No solutions.";
            return result;
        }

        size_t* columnMap = new size_t[num_of_columns - 1];
        moveZeroColumnsRight(columnMap);
        //printMatrix();
        //cout << '\n';

        // Обратный ход
        gaussEliminationBackward();
        printMatrix();
        cout << '\n';

        if ((rank == augmentedRank) && (rank == num_of_columns - 1))
        {
            //auto start = high_resolution_clock::now();

            string* result = new string[num_of_rows];
            for (size_t i = 0; i < num_of_rows; ++i) 
                result[i] = 'x' + to_string(i + 1) + " = " + to_string(data[i][num_of_columns - 1]);

            /*auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << "\n|>> The time is " << duration.count() << " microseconds." << endl;*/

            return result;
        }

        if ((rank == augmentedRank) && (rank < (num_of_columns - 1)))
        {
            string* result = generateFundamentalSystem(rank, columnMap);
            return result;
        }
        delete[] columnMap;
    }

};

// Нахождение максимального возможного элемента матрицы 
long long int calculateMaxElement() 
{
    return static_cast<long long int>(sqrt(LLONG_MAX)) - 1;
}

// Чтение матрицы из файла
Matrix* readMatrixFromFile(const char* filename, size_t num_rows, size_t num_columns, long long int module)
{
    long long maxElement = calculateMaxElement();
    ifstream file(filename);
    if (!file)
    {
        throw "File not found.";
    }

    Matrix* matrix = new Matrix(num_rows, num_columns, module);
    long long int value;
    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < num_columns; ++j)
        {
            if (!(file >> value) || (value > maxElement) || (value < -maxElement)) 
            {
                delete matrix;
                throw "Error: Incorrect input of values from a file.";
            }
            matrix->setElement(i, j, value);
        }
    }
    file.close();
    return matrix;
}

// Генерация строки, соответствующей матрице, заполненной рандомными значениями 
string generateRandomMatrixInModulo(size_t num_rows, size_t num_columns, long long int module) 
{
    // Вычисляем максимально допустимый элемент
    long long int maxElement = calculateMaxElement();

    // Проверяем корректность модуля
    if (module <= 0) 
    {
        throw invalid_argument("Module must be positive and less than or equal to the maximum element.");
    }

    // Настраиваем генератор случайных чисел
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<long long> distribution(0, module - 1);

    ostringstream result;

    // Генерируем случайные числа и формируем строку
    for (size_t i = 0; i < num_rows; ++i) 
    {
        for (size_t j = 0; j < num_columns; ++j) 
        {
            long long int randomValue = distribution(generator);
            result << randomValue;
            if (i != num_rows - 1 || j != num_columns - 1) 
            {
                result << " ";
            }
        }
    }
    return result.str();
}

int main()
{
    try
    {
        long long int module;
        size_t number_of_equations, number_of_variables;
        cout << "Enter the number of equations: ";
        cin >> number_of_equations;
        if (number_of_equations <= 0) throw "Error: Invalid size of matrix side.";

        cout << "Enter the the number of variables: ";
        cin >> number_of_variables;
        if (number_of_variables <= 0) throw "Error: Invalid size of matrix side.";

        cout << "Enter the module: ";
        cin >> module;
        if (module <= 0) throw "Error: Invalid size of matrix side.";

        // Чтение из файла 

        Matrix* matrix = readMatrixFromFile("TextFile1.txt", number_of_equations, (number_of_variables + 1), module);
        
        // Матрица с рандомными значениями

        //string randomMatrixString = generateRandomMatrixInModulo(number_of_equations, number_of_variables + 1, module);
        //Matrix* matrix = new Matrix(number_of_equations, number_of_variables + 1, module);
        //matrix->fromString(randomMatrixString, number_of_equations, number_of_variables + 1, module);

        matrix->printMatrix();
        cout << "\n";

        // Вывод решения 
        string* result = matrix->solve();
        for (size_t i = 0; i < number_of_variables; ++i)
        {
            cout << result[i] << '\n';
            if (result[0] == "No solutions.") break;
        }

        delete[] result;
        return 0;
    }

    catch (const char* error_message)
    {
        cout << "\n|>> " << error_message << endl;
    }

    catch (...)
    {
        cout << "\n|>> Error!" << endl;
    }

}