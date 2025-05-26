#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 100
#define MAX_EDGES 1000
#define DEQUE_EMPTY -1

typedef struct {
    int from;
    int to;
} Edge;

// структура узла
typedef struct Node {
    int data;
    struct Node* prev;
    struct Node* next;
} Node;

// структура дека
typedef struct Deque {
    Node* front;
    Node* rear;
} Deque;

// создание пустого дека
Deque* createDeque() {
    Deque* dq = (Deque*)malloc(sizeof(Deque));
    dq->front = dq->rear = NULL;
    return dq;
}

// проверка на пустоту
int isEmpty(Deque* dq) {
    return dq->front == NULL;
}

// добавление в начало
void pushFront(Deque* dq, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = value;
    newNode->prev = NULL;
    newNode->next = dq->front;

    if (isEmpty(dq))
        dq->rear = newNode;
    else
        dq->front->prev = newNode;

    dq->front = newNode;
}

// добавление в конец
void pushBack(Deque* dq, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = value;
    newNode->next = NULL;
    newNode->prev = dq->rear;

    if (isEmpty(dq))
        dq->front = newNode;
    else
        dq->rear->next = newNode;

    dq->rear = newNode;
}

// удаление из начала
void popFront(Deque* dq) {
    if (isEmpty(dq)) return;
    Node* temp = dq->front;
    dq->front = dq->front->next;
    if (dq->front)
        dq->front->prev = NULL;
    else
        dq->rear = NULL;
    free(temp);
}

// удаление с конца
void popBack(Deque* dq) {
    if (isEmpty(dq)) return;
    Node* temp = dq->rear;
    dq->rear = dq->rear->prev;
    if (dq->rear)
        dq->rear->next = NULL;
    else
        dq->front = NULL;
    free(temp);
}

// просмотр первого элемента
int front(Deque* dq) {
    if (isEmpty(dq)) return DEQUE_EMPTY;
    return dq->front->data;
}

// просмотр последнего элемента
int back(Deque* dq) {
    if (isEmpty(dq)) return DEQUE_EMPTY;
    return dq->rear->data;
}

// очистка всей памяти
void clearDeque(Deque* dq) {
    while (!isEmpty(dq))
        popFront(dq);
    free(dq);
}

// подсчёт количества вершин
int find_vertex_count(Edge *edges, int edge_count) {
    int max = 0;
    for (int i = 0; i < edge_count; ++i) {
        if (edges[i].from > max) max = edges[i].from;
        if (edges[i].to > max) max = edges[i].to;
    }
    return max + 1;
}

// топологическая сортировка методом Кана
void top_sort_kahn(int **adj, int vertex_count) {
    int *in_degree = calloc(vertex_count, sizeof(int));
    int *result = malloc(vertex_count * sizeof(int));
    int count = 0;

    for (int u = 0; u < vertex_count; ++u)
        for (int v = 0; v < vertex_count; ++v)
            if (adj[u][v])
                in_degree[v]++;

    Deque* dq = createDeque();
    for (int i = 0; i < vertex_count; ++i)
        if (in_degree[i] == 0)
            pushBack(dq, i);

    while (!isEmpty(dq)) {
        int u = front(dq);
        popFront(dq);
        result[count++] = u;

        for (int v = 0; v < vertex_count; ++v) {
            if (adj[u][v]) {
                in_degree[v]--;
                if (in_degree[v] == 0)
                    pushBack(dq, v);
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

    clearDeque(dq);
    free(in_degree);
    free(result);
}

// топологическая сортировка методом Тарьяна
int has_cycle = 0;

void dfs_tarjan(int **adj, int u, int *visited, Deque *stack, int vertex_count) {
    if (has_cycle) return;

    visited[u] = 1;

    for (int v = 0; v < vertex_count; ++v) {
        if (!adj[u][v]) continue;
        if (visited[v] == 1) {
            has_cycle = 1;
            return;
        } else if (visited[v] == 0) {
            dfs_tarjan(adj, v, visited, stack, vertex_count);
        }
    }

    visited[u] = 2;
    pushBack(stack, u);
}

void top_sort_tarjan(int **adj, int vertex_count) {
    int *visited = calloc(vertex_count, sizeof(int));
    Deque *stack = createDeque();
    has_cycle = 0;

    for (int i = 0; i < vertex_count; ++i)
        if (!visited[i])
            dfs_tarjan(adj, i, visited, stack, vertex_count);

    if (has_cycle) {
        printf("Граф содержит цикл\n");
    } else {
        printf("Результат (Тарьян): ");
        while (!isEmpty(stack)) {
            printf("%d ", back(stack));
            popBack(stack);
        }
        printf("\n");
    }

    clearDeque(stack);
    free(visited);
}

int main() {
    char filename[100];
    int method = 0;

    printf("Введите имя файла: ");
    scanf("%s", filename);

    // ввод метода сортировки
    while (1) {
        printf("Выберите метод сортировки:\n1 - Кан\n2 - Тарьян\n> ");
        if (scanf("%d", &method) != 1 || (method != 1 && method != 2)) {
            printf("Некорректный ввод. Пожалуйста, введите 1 или 2.\n");
            while (getchar() != '\n');
        } else break;
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
        // пропускаем пустые строки
        if (line[0] == '\n' || line[0] == '\0') continue;

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

    // выделение памяти для матрицы смежности
    int **adj = malloc(vertex_count * sizeof(int *));
    for (int i = 0; i < vertex_count; ++i)
        adj[i] = calloc(vertex_count, sizeof(int));

    // заполнение графа
    for (int i = 0; i < edge_count; ++i) {
        int u = edges[i].from;
        int v = edges[i].to;
        adj[u][v] = 1;
    }

    if (method == 1)
        top_sort_kahn(adj, vertex_count);
    else
        top_sort_tarjan(adj, vertex_count);

    // очистка памяти
    for (int i = 0; i < vertex_count; ++i)
        free(adj[i]);
    free(adj);
    
    return 0;
}
