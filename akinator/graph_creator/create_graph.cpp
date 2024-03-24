#include "create_graph.h"

int go_through_tree(node_t *node, FILE *gcode);
int create_gparh_code();

int main()
{
    OPEN_LOG_FILE();
    fprintf(stderr, "%p\n", graph_err_file);
    if (create_gparh_code())
        return ERR;
    CLOSE_LOG_FILE();

    system("dot graphcode.txt -Tpng -otree.png");
    return NO_ERR;
}

int create_gparh_code()
{
    char *buff = NULL;
    int pos = 0;
    fprintf(stderr, "%p\n", graph_err_file);
    readToBuff("../akinator/data/dataBase.txt", &buff, NULL);
    if (buff == NULL)
    {
        LOG(">>> Can't open database.           [error]");
        return ERR;
    }
    
    node_t *root = readDataBase(buff, &pos);
    free(buff);

    FILE *gcode = fopen("graphcode.txt", "w");
    if (!gcode)
    {
        LOG(">>> graphcode file couldn't open.      [error]\n");
        return ERR;
    }
    setbuf(gcode, NULL);

    fprintf(gcode, GPRAPH_CODE_START);
    go_through_tree(root, gcode);
    LOG(">> tree was read succesfully\n");
    fprintf(gcode, "}\n");

    fclose(gcode);
    treeKill_string(root);

    return NO_ERR;
}

int go_through_tree(node_t *node, FILE *gcode)
{
    assert(node);
    if (!node->left && !node->right)
    {
        LOG("> the end of the tree found, the object is: %s\n", node->data);
        return 1;
    }

    if (node->left)
    {
        fprintf(gcode, "\t\"%s\" -> \"%s\" [color = \"#478056\"];\n", node->data, node->left->data);
        go_through_tree(node->left, gcode);
        LOG("> left tree found with the string: %s\n", node->left->data);
    }
    if (node->right)
    {
        fprintf(gcode, "\t\"%s\" -> \"%s\" [color = \"#632b2b\"];\n", node->data, node->right->data);
        go_through_tree(node->right, gcode);
        LOG("> right tree found with the string: %s\n", node->right->data);
    }
    
    return 0;
}