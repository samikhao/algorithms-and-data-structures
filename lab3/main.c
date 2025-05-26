#include <stdio.h>
#include <stdlib.h>

#define MAX_EDGES 1000

typedef struct {
    int from;
    int to;
} Edge;

// структура узла кольцевой очереди
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// структура кольцевой очереди
typedef struct {
    Node* head;
    Node* tail;
    int size;
} Queue;

// инициализация пустой очереди
void init_queue(Queue* Q) {
    Q->head = NULL;
    Q->tail = NULL;
    Q->size = 0;
}

// проверка на пустоту
int is_empty(Queue* Q) {
    return Q->size == 0;
}

// добавление в конец
void enqueue(Queue* Q, int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = value;

    if (is_empty(Q)) {
        new_node->next = new_node;
        Q->head = Q->tail = new_node;
    } else {
        new_node->next = Q->head;
        Q->tail->next = new_node;
        Q->tail = new_node;
    }
    Q->size++;
}

// удаление из начала
int dequeue(Queue* Q) {
    if (is_empty(Q)) {
        printf("Очередь пуста\n");
        exit(1);
    }

    Node* to_delete = Q->head;
    int value = to_delete->data;

    if (Q->head == Q->tail) {
        Q->head = Q->tail = NULL;
    } else {
        Q->head = Q->head->next;
        Q->tail->next = Q->head;
    }

    free(to_delete);
    Q->size--;

    return value;
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
    Queue Q;
    init_queue(&Q);

    for (int u = 0; u < vertex_count; ++u)
        for (int v = 0; v < vertex_count; ++v)
            if (adj[u][v])
                in_degree[v]++;

    for (int i = 0; i < vertex_count; ++i)
        if (in_degree[i] == 0)
            enqueue(&Q, i);

    while (!is_empty(&Q)) {
        int u = dequeue(&Q);
        result[count++] = u;

        for (int v = 0; v < vertex_count; ++v) {
            if (adj[u][v]) {
                in_degree[v]--;
                if (in_degree[v] == 0)
                    enqueue(&Q, v);
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
    free(result);
}

// топологическая сортировка методом Тарьяна
int has_cycle = 0;

void dfs_tarjan(int **adj, int u, int *visited, Queue *stack, int vertex_count) {
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
    enqueue(stack, u);
}

// выводит элементы очереди в обратном порядке
void print_stack_reverse(Queue *stack) {
    if (stack->size == 0) return;

    int *arr = malloc(stack->size * sizeof(int));
    int i = 0;
    while (!is_empty(stack)) {
        arr[i++] = dequeue(stack);
    }
    for (int j = i - 1; j >= 0; --j)
        printf("%d ", arr[j]);
    printf("\n");
    free(arr);
}

void top_sort_tarjan(int **adj, int vertex_count) {
    int *visited = calloc(vertex_count, sizeof(int));
    Queue stack;
    init_queue(&stack);
    has_cycle = 0;

    for (int i = 0; i < vertex_count; ++i)
        if (!visited[i])
            dfs_tarjan(adj, i, visited, &stack, vertex_count);

    if (has_cycle) {
        printf("Граф содержит цикл\n");
    } else {
        printf("Результат (Тарьян): ");
        print_stack_reverse(&stack);
    }

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
