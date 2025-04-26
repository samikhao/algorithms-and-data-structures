#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 100
#define MAX_EDGES 1000

typedef struct {
    int from;
    int to;
} Edge;

// Подсчёт количества вершин
int find_vertex_count(Edge *edges, int edge_count) {
    int max = 0;
    for (int i = 0; i < edge_count; ++i) {
        if (edges[i].from > max) max = edges[i].from;
        if (edges[i].to > max) max = edges[i].to;
    }
    return max + 1;
}

// Топологическая сортировка методом Кана
void top_sort_kahn(int **adj, int vertex_count) {
    int *in_degree = calloc(vertex_count, sizeof(int));
    int *queue = malloc(vertex_count * sizeof(int));
    int *result = malloc(vertex_count * sizeof(int));
    int front = 0, rear = 0, count = 0;

    for (int u = 0; u < vertex_count; ++u)
        for (int v = 0; v < vertex_count; ++v)
            if (adj[u][v])
                in_degree[v]++;

    for (int i = 0; i < vertex_count; ++i)
        if (in_degree[i] == 0)
            queue[rear++] = i;

    while (front < rear) {
        int u = queue[front++];
        result[count++] = u;

        for (int v = 0; v < vertex_count; ++v) {
            if (adj[u][v]) {
                in_degree[v]--;
                if (in_degree[v] == 0)
                    queue[rear++] = v;
            }
        }
    }

    if (count != vertex_count) {
        printf("Граф содержит цикл\n");
    } else {
        printf("Результат (Кан): ");
        for (int i = 0; i < count; ++i)
            printf("%d ", result[i]);
        printf("\n");
    }

    free(in_degree);
    free(queue);
    free(result);
}

// Топологическая сортировка методом Тарьяна
int has_cycle = 0;

void dfs_tarjan(int **adj, int u, int *visited, int *stack, int *top, int vertex_count) {
    if (has_cycle) return;

    visited[u] = 1;

    for (int v = 0; v < vertex_count; ++v) {
        if (!adj[u][v]) continue;
        if (visited[v] == 1) {
            has_cycle = 1;
            return;
        } else if (visited[v] == 0) {
            dfs_tarjan(adj, v, visited, stack, top, vertex_count);
        }
    }

    visited[u] = 2;
    stack[(*top)++] = u;
}

void top_sort_tarjan(int **adj, int vertex_count) {
    int *visited = calloc(vertex_count, sizeof(int));
    int *stack = malloc(vertex_count * sizeof(int));
    int top = 0;
    has_cycle = 0;

    for (int i = 0; i < vertex_count; ++i)
        if (!visited[i])
            dfs_tarjan(adj, i, visited, stack, &top, vertex_count);

    if (has_cycle) {
        printf("Граф содержит цикл\n");
    } else {
        printf("Результат (Тарьян): ");
        for (int i = top - 1; i >= 0; --i)
            printf("%d ", stack[i]);
        printf("\n");
    }

    free(visited);
    free(stack);
}

int main() {
    char filename[100];

    printf("Введите имя файла: ");
    scanf("%s", filename);

    int method = 0, storage = 0;

    // Ввод метода сортировки
    while (1) {
        printf("Выберите метод сортировки:\n1 - Кан\n2 - Тарьян\n> ");
        if (scanf("%d", &method) != 1 || (method != 1 && method != 2)) {
            printf("Некорректный ввод. Пожалуйста, введите 1 или 2.\n");
            while (getchar() != '\n'); // очистка ввода
        } else {
            break;
        }
    }

    // Ввод типа хранения
    while (1) {
        printf("Выберите тип массива:\n1 - Статический\n2 - Динамический\n> ");
        if (scanf("%d", &storage) != 1 || (storage != 1 && storage != 2)) {
            printf("Некорректный ввод. Пожалуйста, введите 1 или 2.\n");
            while (getchar() != '\n'); // очистка ввода
        } else {
            break;
        }
    }


    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Ошибка при открытии файла");
        return 1;
    }

    Edge edges[MAX_EDGES];
    int edge_count = 0;
    char line[256];
    int line_number = 0;

    while (fgets(line, sizeof(line), f)) {
        line_number++;

        // Пропускаем пустые строки
        if (line[0] == '\n' || line[0] == '\0')
            continue;

        int u, v;
        char extra;

        int count = sscanf(line, "%d %d %c", &u, &v, &extra);
        if (count < 2 || u < 0 || v < 0) {
            printf("Ошибка в строке %d: '%s'\n", line_number, line);
            continue; // пропускаем некорректную строку
        }

        if (edge_count >= MAX_EDGES) {
            printf("Превышено допустимое количество рёбер (%d)\n", MAX_EDGES);
            break;
        }

        edges[edge_count].from = u;
        edges[edge_count].to = v;
        edge_count++;
    }
    fclose(f);

    if (edge_count == 0) {
        printf("Файл не содержит корректных рёбер. Завершение программы.\n");
        return 1;
    }

    int vertex_count = find_vertex_count(edges, edge_count);

    // Выделение памяти для матрицы смежности
    int **adj = NULL;
    static int static_adj[MAX_VERTICES][MAX_VERTICES] = {0};

    if (storage == 1) { 
        // Статический массив
        adj = (int **)malloc(vertex_count * sizeof(int *));
        for (int i = 0; i < vertex_count; ++i)
            adj[i] = static_adj[i];
    } else {
        // Динамический массив
        adj = (int **)malloc(vertex_count * sizeof(int *));
        for (int i = 0; i < vertex_count; ++i)
            adj[i] = (int *)calloc(vertex_count, sizeof(int));
    }

    // Заполнение графа
    for (int i = 0; i < edge_count; ++i) {
        int u = edges[i].from;
        int v = edges[i].to;
        adj[u][v] = 1;
    }

    // Запуск нужного метода
    if (method == 1)
        top_sort_kahn(adj, vertex_count);
    else
        top_sort_tarjan(adj, vertex_count);

    // Очистка
    if (storage == 2) {
        for (int i = 0; i < vertex_count; ++i)
            free(adj[i]);
    }
    free(adj);

    return 0;
}
