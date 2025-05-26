#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EDGES 1000
#define ALPHABET_SIZE 256

typedef struct {
    int from;
    int to;
} Edge;

typedef enum { RED, BLACK } Color;

// узел красно-чёрного дерева
typedef struct TreeNode {
    long long data;
    Color color;
    struct TreeNode *left, *right, *parent;
} TreeNode;

// красно-чёрное дерево
typedef struct {
    TreeNode* root;
    TreeNode* nil;
} RBTree;

// инициализация дерева
void init_rbtree(RBTree* tree) {
    tree->nil = (TreeNode*)malloc(sizeof(TreeNode));
    tree->nil->color = BLACK;
    tree->nil->left = tree->nil->right = tree->nil->parent = NULL;
    tree->root = tree->nil;
}

// поворот поддерева влево
void left_rotate(RBTree* tree, TreeNode* x) {
    TreeNode* y = x->right;
    x->right = y->left;

    if (y->left != tree->nil)
        y->left->parent = x;

    y->parent = x->parent;

    if (x->parent == tree->nil)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

// поворот поддерева вправо
void right_rotate(RBTree* tree, TreeNode* y) {
    TreeNode* x = y->left;
    y->left = x->right;

    if (x->right != tree->nil)
        x->right->parent = y;

    x->parent = y->parent;

    if (y->parent == tree->nil)
        tree->root = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left = x;

    x->right = y;
    y->parent = x;
}

// восстановление свойств дерева после вставки
void insert_fixup(RBTree* tree, TreeNode* z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            TreeNode* y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(tree, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(tree, z->parent->parent);
            }
        } else {
            TreeNode* y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(tree, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

// вставка элемента
void insert_rbtree(RBTree* tree, long long value) {
    TreeNode* z = (TreeNode*)malloc(sizeof(TreeNode));
    z->data = value;
    z->color = RED;
    z->left = z->right = z->parent = tree->nil;

    TreeNode* y = tree->nil;
    TreeNode* x = tree->root;

    while (x != tree->nil) {
        y = x;
        if (z->data < x->data)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;
    if (y == tree->nil)
        tree->root = z;
    else if (z->data < y->data)
        y->left = z;
    else
        y->right = z;

    insert_fixup(tree, z);
}

// создание таблицы плохих символов
void bad_char(const char* pat, int m, int badchar[ALPHABET_SIZE]) {
    for (int i = 0; i < ALPHABET_SIZE; i++)
        badchar[i] = -1;
    for (int i = 0; i < m; i++)
        badchar[(unsigned char)pat[i]] = i;
}

// поиск границ хороших суффиксов
void good_suffix_borders(int* shift, int* bpos, const char* pat, int m) {
    int i = m, j = m + 1;
    bpos[i] = j;
    while (i > 0) {
        while (j <= m && pat[i - 1] != pat[j - 1]) {
            if (shift[j] == 0)
                shift[j] = j - i;
            j = bpos[j];
        }
        i--;
        j--;
        bpos[i] = j;
    }
}

// создание таблицы хороших суффиксов
void good_suffix(int* shift, int* bpos, const char* pat, int m) {
    for (int i = 0; i <= m; i++)
        shift[i] = bpos[i] = 0;

    good_suffix_borders(shift, bpos, pat, m);

    int j = bpos[0];
    for (int i = 0; i <= m; i++) {
        if (shift[i] == 0)
            shift[i] = j;
        if (i == j)
            j = bpos[j];
    }
}

// поиск Бойера-Мура
int boyer_moore_search(const char* text, const char* pattern) {
    int m = strlen(pattern), n = strlen(text);
    if (m == 0 || n < m) return -1;

    int badchar[ALPHABET_SIZE];
    bad_char(pattern, m, badchar);

    int* bpos = malloc((m + 1) * sizeof(int));
    int* shift = malloc((m + 1) * sizeof(int));
    good_suffix(shift, bpos, pattern, m);

    int s = 0;
    while (s <= n - m) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[s + j]) j--;
        if (j < 0) {
            free(bpos); free(shift);
            return s;
        }
        int bad_shift = j - badchar[(unsigned char)text[s + j]];
        int good_shift = shift[j + 1];
        s += (bad_shift > good_shift) ? bad_shift : good_shift;
    }
    free(bpos);
    free(shift);
    return -1;
}

// подсветка найденного совпадения
void highlight_match(const char* text, int pos, const char* pattern) {
    for (int i = 0; i < pos; i++)
        printf("%c", text[i]);
    printf("\x1b[32m");
    printf("%.*s", (int)strlen(pattern), text + pos);
    printf("\x1b[0m");
    printf("%s\n", text + pos + strlen(pattern));
}

// включает в себя поиск и запись массива в дерево
void process_and_search(int *result, int count) {
    RBTree tree;
    init_rbtree(&tree);
    TreeNode** inserted_nodes = malloc(count * sizeof(TreeNode*));

    char text[1024] = "";
    char buffer[32];
    int len = 0;

    for (int i = 0; i < count; i++) {
        if (i > 0)
            text[len++] = ' ';
        int written = snprintf(buffer, sizeof(buffer), "%d", result[i]);
        strcpy(&text[len], buffer);
        len += written;

        // Вставка с сохранением ссылки на узел
        TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
        node->data = result[i];
        node->color = RED;
        node->left = node->right = node->parent = tree.nil;

        TreeNode* y = tree.nil;
        TreeNode* x = tree.root;
        while (x != tree.nil) {
            y = x;
            if (node->data < x->data)
                x = x->left;
            else
                x = x->right;
        }
        node->parent = y;
        if (y == tree.nil)
            tree.root = node;
        else if (node->data < y->data)
            y->left = node;
        else
            y->right = node;

        insert_fixup(&tree, node);
        inserted_nodes[i] = node;
    }
    text[len] = '\0';

    printf("Введите подстроку для поиска: ");

    // очистка буфера после предыдущего scanf
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    char pattern[100];
    fgets(pattern, sizeof(pattern), stdin);

    // удаление символа новой строки, если он остался
    size_t pattern_len = strlen(pattern);
    if (pattern_len > 0 && pattern[pattern_len - 1] == '\n')
        pattern[pattern_len - 1] = '\0';

    int pos = boyer_moore_search(text, pattern);
    if (pos >= 0) {
        printf("Найдено совпадение: \n");
        highlight_match(text, pos, pattern);
    } else {
        printf("Совпадений не найдено.\n");
    }

    printf("Результат в виде красно-чёрного дерева: \n");
    for (int i = 0; i < count; i++) {
        printf("%lld ", inserted_nodes[i]->data);
    }
    printf("\n");

    free(inserted_nodes);
}

// подсчёт количества вершин
int find_vertex_count(Edge *edges, int edge_count) {
    int max = 0;
    for (int i = 0; i < edge_count; ++i) {
        if (edges[i].from > max)
            max = edges[i].from;
        if (edges[i].to > max)
            max = edges[i].to;
    }
    return max + 1;
}

// топологическая сортировка методом Кана
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
        process_and_search(result, count);
    }

    free(in_degree);
    free(queue);
    free(result);
}

// топологическая сортировка методом Тарьяна
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
        int* result = malloc(top * sizeof(int));
        printf("Результат (Тарьян): ");
        for (int i = 0; i < top; ++i) {
            result[i] = stack[top - 1 - i]; // разворачиваем стек
            printf("%d ", result[i]);
        }
        printf("\n");
        process_and_search(result, top);
        free(result);
    }

    free(visited);
    free(stack);
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
            while (getchar() != '\n'); // очистка ввода
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

    // запуск нужного метода
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