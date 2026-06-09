#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS 20
#define COLS 50
#define BG_CHAR '_'
#define FG_CHAR '*'
#define MAX_SHAPES 100

// ==========================================
//           DATA STRUCTURE DEFINITIONS
// ==========================================

typedef enum {
    SHAPE_LINE,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

typedef struct {
    int x1, y1;
    int x2, y2;
} LineData;

typedef struct {
    int x, y; // Top-left corner
    int width, height;
} RectangleData;

typedef struct {
    int cx, cy; // Center
    int radius;
} CircleData;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriangleData;

typedef struct {
    int id;
    ShapeType type;
    union {
        LineData line;
        RectangleData rect;
        CircleData circle;
        TriangleData triangle;
    } data;
} Shape;

// Global State
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_id = 1;

// ==========================================
//          GRAPHICS & DRAWING LOGIC
// ==========================================

void clear_canvas(char canvas[ROWS][COLS]) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            canvas[r][c] = BG_CHAR;
        }
    }
}

void draw_pixel(char canvas[ROWS][COLS], int x, int y) {
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
        canvas[y][x] = FG_CHAR;
    }
}

// Bresenham's Line Drawing Algorithm
void draw_line(char canvas[ROWS][COLS], int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        draw_pixel(canvas, x0, y0);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void draw_rectangle(char canvas[ROWS][COLS], int x, int y, int width, int height) {
    if (width <= 0 || height <= 0) return;
    
    // Top and bottom edges
    for (int i = 0; i < width; ++i) {
        draw_pixel(canvas, x + i, y);
        draw_pixel(canvas, x + i, y + height - 1);
    }
    // Left and right edges
    for (int i = 0; i < height; ++i) {
        draw_pixel(canvas, x, y + i);
        draw_pixel(canvas, x + width - 1, y + i);
    }
}

// Midpoint Circle Drawing Algorithm
void draw_circle(char canvas[ROWS][COLS], int cx, int cy, int radius) {
    if (radius < 0) return;
    if (radius == 0) {
        draw_pixel(canvas, cx, cy);
        return;
    }
    
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    while (y >= x) {
        draw_pixel(canvas, cx + x, cy + y);
        draw_pixel(canvas, cx - x, cy + y);
        draw_pixel(canvas, cx + x, cy - y);
        draw_pixel(canvas, cx - x, cy - y);
        draw_pixel(canvas, cx + y, cy + x);
        draw_pixel(canvas, cx - y, cy + x);
        draw_pixel(canvas, cx + y, cy - x);
        draw_pixel(canvas, cx - y, cy - x);
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void draw_triangle(char canvas[ROWS][COLS], int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(canvas, x1, y1, x2, y2);
    draw_line(canvas, x2, y2, x3, y3);
    draw_line(canvas, x3, y3, x1, y1);
}

void render_canvas(char canvas[ROWS][COLS], const Shape *shapes_list, int count) {
    clear_canvas(canvas);
    for (int i = 0; i < count; ++i) {
        const Shape *s = &shapes_list[i];
        switch (s->type) {
            case SHAPE_LINE:
                draw_line(canvas, s->data.line.x1, s->data.line.y1, s->data.line.x2, s->data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(canvas, s->data.rect.x, s->data.rect.y, s->data.rect.width, s->data.rect.height);
                break;
            case SHAPE_CIRCLE:
                draw_circle(canvas, s->data.circle.cx, s->data.circle.cy, s->data.circle.radius);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(canvas, s->data.triangle.x1, s->data.triangle.y1, 
                              s->data.triangle.x2, s->data.triangle.y2, 
                              s->data.triangle.x3, s->data.triangle.y3);
                break;
        }
    }
}

void display_canvas(const char canvas[ROWS][COLS]) {
    // Print column headers (tens digits)
    printf("   ");
    for (int c = 0; c < COLS; ++c) {
        if (c >= 10) {
            printf("%d", c / 10);
        } else {
            printf(" ");
        }
    }
    printf("\n");

    // Print column headers (units digits)
    printf("   ");
    for (int c = 0; c < COLS; ++c) {
        printf("%d", c % 10);
    }
    printf("\n");

    // Top border
    printf("  +");
    for (int c = 0; c < COLS; ++c) {
        putchar('-');
    }
    printf("+\n");

    // Canvas contents with row indices
    for (int r = 0; r < ROWS; ++r) {
        printf("%2d|", r);
        for (int c = 0; c < COLS; ++c) {
            putchar(canvas[r][c]);
        }
        printf("|\n");
    }

    // Bottom border
    printf("  +");
    for (int c = 0; c < COLS; ++c) {
        putchar('-');
    }
    printf("+\n");
}

const char* shape_type_to_string(ShapeType type) {
    switch (type) {
        case SHAPE_LINE:      return "Line";
        case SHAPE_RECTANGLE: return "Rectangle";
        case SHAPE_CIRCLE:    return "Circle";
        case SHAPE_TRIANGLE:  return "Triangle";
        default:              return "Unknown";
    }
}

// ==========================================
//        INTERACTIVE MENU & SELECTIONS
// ==========================================

// Helper function to safely read an integer in a given range
int get_int_input(const char* prompt, int min_val, int max_val) {
    int val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        
        if (sscanf(buffer, "%d", &val) == 1) {
            if (val >= min_val && val <= max_val) {
                return val;
            }
        }
        printf("Invalid input. Please enter an integer between %d and %d.\n", min_val, max_val);
    }
}

void list_shapes() {
    if (shape_count == 0) {
        printf("\n--- No shapes in the picture ---\n");
        return;
    }
    printf("\n--- Shapes in the Picture ---\n");
    for (int i = 0; i < shape_count; ++i) {
        const Shape *s = &shapes[i];
        printf("ID [%d]: %s - ", s->id, shape_type_to_string(s->type));
        switch (s->type) {
            case SHAPE_LINE:
                printf("Endpoint 1: (%d, %d), Endpoint 2: (%d, %d)\n", 
                       s->data.line.x1, s->data.line.y1, s->data.line.x2, s->data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                printf("Top-Left: (%d, %d), Width: %d, Height: %d\n", 
                       s->data.rect.x, s->data.rect.y, s->data.rect.width, s->data.rect.height);
                break;
            case SHAPE_CIRCLE:
                printf("Center: (%d, %d), Radius: %d\n", 
                       s->data.circle.cx, s->data.circle.cy, s->data.circle.radius);
                break;
            case SHAPE_TRIANGLE:
                printf("Vertices: (%d, %d), (%d, %d), (%d, %d)\n", 
                       s->data.triangle.x1, s->data.triangle.y1, 
                       s->data.triangle.x2, s->data.triangle.y2, 
                       s->data.triangle.x3, s->data.triangle.y3);
                break;
        }
    }
}

int find_shape_index(int id) {
    for (int i = 0; i < shape_count; ++i) {
        if (shapes[i].id == id) {
            return i;
        }
    }
    return -1;
}

void add_shape() {
    if (shape_count >= MAX_SHAPES) {
        printf("Error: Maximum shape limit reached (%d).\n", MAX_SHAPES);
        return;
    }

    printf("\n--- Add a New Shape ---\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    int choice = get_int_input("Choose shape type (1-4): ", 1, 4);

    Shape s;
    s.id = next_id++;
    
    switch (choice) {
        case 1:
            s.type = SHAPE_LINE;
            printf("\nEnter endpoints for the line:\n");
            s.data.line.x1 = get_int_input("  Start X (0-49): ", 0, COLS - 1);
            s.data.line.y1 = get_int_input("  Start Y (0-19): ", 0, ROWS - 1);
            s.data.line.x2 = get_int_input("  End X (0-49): ", 0, COLS - 1);
            s.data.line.y2 = get_int_input("  End Y (0-19): ", 0, ROWS - 1);
            break;
            
        case 2:
            s.type = SHAPE_RECTANGLE;
            printf("\nEnter top-left corner and dimensions for the rectangle:\n");
            s.data.rect.x = get_int_input("  Top-Left X (0-49): ", 0, COLS - 1);
            s.data.rect.y = get_int_input("  Top-Left Y (0-19): ", 0, ROWS - 1);
            s.data.rect.width = get_int_input("  Width (1-50): ", 1, COLS);
            s.data.rect.height = get_int_input("  Height (1-20): ", 1, ROWS);
            break;
            
        case 3:
            s.type = SHAPE_CIRCLE;
            printf("\nEnter center and radius for the circle:\n");
            s.data.circle.cx = get_int_input("  Center X (0-49): ", 0, COLS - 1);
            s.data.circle.cy = get_int_input("  Center Y (0-19): ", 0, ROWS - 1);
            s.data.circle.radius = get_int_input("  Radius (0-30): ", 0, 30);
            break;
            
        case 4:
            s.type = SHAPE_TRIANGLE;
            printf("\nEnter vertices for the triangle:\n");
            s.data.triangle.x1 = get_int_input("  Vertex 1 X (0-49): ", 0, COLS - 1);
            s.data.triangle.y1 = get_int_input("  Vertex 1 Y (0-19): ", 0, ROWS - 1);
            s.data.triangle.x2 = get_int_input("  Vertex 2 X (0-49): ", 0, COLS - 1);
            s.data.triangle.y2 = get_int_input("  Vertex 2 Y (0-19): ", 0, ROWS - 1);
            s.data.triangle.x3 = get_int_input("  Vertex 3 X (0-49): ", 0, COLS - 1);
            s.data.triangle.y3 = get_int_input("  Vertex 3 Y (0-19): ", 0, ROWS - 1);
            break;
    }

    shapes[shape_count++] = s;
    printf("Shape successfully added with ID: %d!\n", s.id);
}

void delete_shape() {
    if (shape_count == 0) {
        printf("No shapes available to delete.\n");
        return;
    }
    
    int id = get_int_input("Enter the ID of the shape to delete: ", 1, 10000);
    int index = find_shape_index(id);
    
    if (index == -1) {
        printf("Error: Shape with ID %d not found.\n", id);
        return;
    }
    
    // Shift subsequent shapes down
    for (int i = index; i < shape_count - 1; ++i) {
        shapes[i] = shapes[i + 1];
    }
    shape_count--;
    printf("Shape with ID %d successfully deleted!\n", id);
}

void modify_shape() {
    if (shape_count == 0) {
        printf("No shapes available to modify.\n");
        return;
    }
    
    int id = get_int_input("Enter the ID of the shape to modify: ", 1, 10000);
    int index = find_shape_index(id);
    
    if (index == -1) {
        printf("Error: Shape with ID %d not found.\n", id);
        return;
    }
    
    Shape *s = &shapes[index];
    printf("\nModifying shape ID [%d] (%s):\n", s->id, shape_type_to_string(s->type));
    
    switch (s->type) {
        case SHAPE_LINE:
            printf("Current: Endpoint 1: (%d, %d), Endpoint 2: (%d, %d)\n", 
                   s->data.line.x1, s->data.line.y1, s->data.line.x2, s->data.line.y2);
            s->data.line.x1 = get_int_input("  New Start X (0-49): ", 0, COLS - 1);
            s->data.line.y1 = get_int_input("  New Start Y (0-19): ", 0, ROWS - 1);
            s->data.line.x2 = get_int_input("  New End X (0-49): ", 0, COLS - 1);
            s->data.line.y2 = get_int_input("  New End Y (0-19): ", 0, ROWS - 1);
            break;
            
        case SHAPE_RECTANGLE:
            printf("Current: Top-Left: (%d, %d), Width: %d, Height: %d\n", 
                   s->data.rect.x, s->data.rect.y, s->data.rect.width, s->data.rect.height);
            s->data.rect.x = get_int_input("  New Top-Left X (0-49): ", 0, COLS - 1);
            s->data.rect.y = get_int_input("  New Top-Left Y (0-19): ", 0, ROWS - 1);
            s->data.rect.width = get_int_input("  New Width (1-50): ", 1, COLS);
            s->data.rect.height = get_int_input("  New Height (1-20): ", 1, ROWS);
            break;
            
        case SHAPE_CIRCLE:
            printf("Current: Center: (%d, %d), Radius: %d\n", 
                   s->data.circle.cx, s->data.circle.cy, s->data.circle.radius);
            s->data.circle.cx = get_int_input("  New Center X (0-49): ", 0, COLS - 1);
            s->data.circle.cy = get_int_input("  New Center Y (0-19): ", 0, ROWS - 1);
            s->data.circle.radius = get_int_input("  New Radius (0-30): ", 0, 30);
            break;
            
        case SHAPE_TRIANGLE:
            printf("Current: Vertices: (%d, %d), (%d, %d), (%d, %d)\n", 
                   s->data.triangle.x1, s->data.triangle.y1, 
                   s->data.triangle.x2, s->data.triangle.y2, 
                   s->data.triangle.x3, s->data.triangle.y3);
            s->data.triangle.x1 = get_int_input("  New Vertex 1 X (0-49): ", 0, COLS - 1);
            s->data.triangle.y1 = get_int_input("  New Vertex 1 Y (0-19): ", 0, ROWS - 1);
            s->data.triangle.x2 = get_int_input("  New Vertex 2 X (0-49): ", 0, COLS - 1);
            s->data.triangle.y2 = get_int_input("  New Vertex 2 Y (0-19): ", 0, ROWS - 1);
            s->data.triangle.x3 = get_int_input("  New Vertex 3 X (0-49): ", 0, COLS - 1);
            s->data.triangle.y3 = get_int_input("  New Vertex 3 Y (0-19): ", 0, ROWS - 1);
            break;
    }
    printf("Shape with ID %d successfully modified!\n", id);
}

int main() {
    char canvas[ROWS][COLS];
    
    printf("==========================================\n");
    printf("       2D C GRAPHICS EDITOR (CLI)         \n");
    printf("==========================================\n");
    
    while (1) {
        render_canvas(canvas, shapes, shape_count);
        display_canvas(canvas);
        list_shapes();
        
        printf("\nMenu:\n");
        printf("1. Add Shape\n");
        printf("2. Delete Shape\n");
        printf("3. Modify Shape\n");
        printf("4. Clear Canvas (Delete All)\n");
        printf("5. Exit\n");
        
        int option = get_int_input("Select menu option (1-5): ", 1, 5);
        
        if (option == 1) {
            add_shape();
        } else if (option == 2) {
            delete_shape();
        } else if (option == 3) {
            modify_shape();
        } else if (option == 4) {
            shape_count = 0;
            printf("All shapes deleted. Canvas cleared.\n");
        } else if (option == 5) {
            printf("Exiting Graphics Editor. Goodbye!\n");
            break;
        }
        printf("\nPress Enter to continue...\n");
        getchar();
    }
    
    return 0;
}
