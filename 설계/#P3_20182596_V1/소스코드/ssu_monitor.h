//#include <sys/stat.h>
#define BUFLEN 1024
#define PATHLEN 4096

typedef struct tree_node{
	char path[PATHLEN];
	char name[PATHLEN];
	bool isEnd;
	bool isDir;
	mode_t mode;
	time_t mtime;
	struct tree_node *child;
	struct tree_node *parent;
	struct tree_node *prev;
	struct tree_node *next;
} tree;

void ssu_prompt(void);
int execute_command(int argc, char *argv[]);
void execute_add(int argc, char *argv[]);
void execute_delete(int argc, char *argv[]);
void execute_tree(int argc, char *argv[]);
void execute_tab(tree *head);
void print_tree(tree *cur);
void __execute_tree(tree *node, int level);
void execute_help(int argc, char *argv[]);
void execute_exit(int argc, char *argv[]);
void init_daemon(char *dirpath, time_t mn_time);
pid_t make_daemon(void);
tree *create_node(char *path, mode_t mode, time_t mtime, char *name);
void make_tree(tree **dir, char *path);
void make_tree2(tree **dir, char *path);
void compare_tree(tree* head, tree* data, char dirPath[], time_t mn_time, int fd);
void compare_tree2(tree* old_tree, tree *new_tree, char dirPath[],int fd);
tree *compare_node(tree *data, char name[]);
void write_log(char message[], char daystr[], char *str, tree *data, int fd);
void print_tree(tree *node);
void free_tree(tree *cur);
void signal_handler(int signum);
int scandir_filter(const struct dirent *file);
