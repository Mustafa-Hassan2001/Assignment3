/*
 * ass3.c
 *
 *  Created on: Dec 8, 2017
 *      Author:
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Define the struct*/
typedef struct point
{
    int index; /* the order in the instance file           */
    int x; /* x coordinate                             */
    int y; /* y coordinate                             */
    int parent; /* parent in the tree when added            */
    int num_children; /* has value 0-8                            */
    int child[8];
    int overlap_hv; /* total overlap when horizontal then vertical  */
    int overlap_vh; /* total overlap when the other way             */
    int layout;
} point;

typedef struct single_point
{
    int x; /* x coordinate */
    int y; /* y coordinate */
} single_point;

/* Global variable*/
int global_hv_overlap = 0;
int global_vh_overlap = 0;

/* Prototype function*/
int parse_command(int argc, char **argv, char **inputfile_name, char **outputfile_name, int* outfile_position);
int assignment_process(char *inputfile_name, char* outputfile_name, int outputfile_pos);
int print_point(point *p, FILE *fp);
int dfs_mst(point *p, int index, int *dfs_order, FILE *fp);
int *reverse_dfs(int* dfs_order, int n);
int tree_calculate(point *p, int *reversed_dfs, int n);
void combination(int *parent_layout, point *p, int index, int num_children, int n);
int overlap_calculate(int *parent_layout, point *p, int index);
int if_l_shape(point *p, int current_index);
int h_or_v(point *p, int current_index);
int mst_calculate(int *x_coordinate, int *y_coordinate, int NUM_PT, int MAX_X, int MAX_Y, FILE *fp, FILE *infp);

int main(int argc, char* argv[])
{
    char* inputfile_name;
    char* outputfile_name;
    int outfile_position;

    if (parse_command(argc, argv, &inputfile_name, &outputfile_name, &outfile_position) == 1)
    {
        printf("Invalid running.\n");
        printf("\tUse: %s -i inputfile [-o outputfile]\n", argv[0]);
        printf("\tExample1: %s -i instance10_002.txt \n", argv[0]);
        printf("\tExample2: %s -i instance10_002.txt -o output10 002.txt\n", argv[0]);
        return 1;
    }

    if (assignment_process(inputfile_name, outputfile_name, outfile_position) == 1)
    {
        printf("Error in reading the instance file!\n");
        return 3;
    }
    return 0;
}

/*
 * assignment_process() -
 */
int assignment_process(char *inputfile_name, char*outputfile_name, int outputfile_pos)
{
    FILE *fp;

    int MAX_X, MAX_Y, NUM_PT;

    if ((fp = fopen(inputfile_name, "r")) == NULL)
        return 1;

    while (fscanf(fp, "%d", &MAX_X) != 1)
    {
        if (ferror(fp))
        {
            fclose(fp);
            return 1;
        }
        if (feof(fp))
        {
            fclose(fp);
            return 1;
        }
        fscanf(fp, "%*[^\n]");
    }
    if (fscanf(fp, "%d", &MAX_Y) != 1)
    {
        fclose(fp);
        return 1;
    }

    while (fscanf(fp, "%d", &NUM_PT) != 1)
    {

        if (ferror(fp))
        {
            fclose(fp);
            return 1;
        }
        if (feof(fp))
        {
            fclose(fp);
            return 1;
        }

        fscanf(fp, "%*[^\n]");
    }

    /* if there is only one point. terminate. */
    if (NUM_PT == 1)
    {
        return 1;
    }

    /**************** read the cordinates *************************/
    int *x_coordinate;
    int *y_coordinate;
    x_coordinate = (int *) malloc(NUM_PT * sizeof(int));
    y_coordinate = (int *) malloc(NUM_PT * sizeof(int));
    int i;
    for (i = 0; i < NUM_PT; i++)
    {
        while (fscanf(fp, "%d", &x_coordinate[i]) != 1)
        {

            if (ferror(fp))
            {
                fclose(fp);
                return 1;
            }
            if (feof(fp))
            {
                fclose(fp);
                return 1;
            }
            fscanf(fp, "%*[^\n]");
        }
        if (fscanf(fp, "%d", &y_coordinate[i]) != 1)
        {
            fclose(fp);
            return 1;
        }
    }
    int extra_point_x, extra_point_y; /* if there is more than 10 points , error */
    if (fscanf(fp, "%d %d", &extra_point_x, &extra_point_y) == 2)
        return 1;
    /*check the input dots*/
    for (i = 0; i < NUM_PT; i++)
    {
        /*out of the area*/
        if (x_coordinate[i] > MAX_X || y_coordinate[i] > MAX_Y)
        {
            return 1;
        }

    }

    /*check no duplicates*/

    int j;
    for (i = 0; i < NUM_PT; i++)
    {
        for (j = i + 1; j < NUM_PT; j++)
        {
            if ((x_coordinate[i] == x_coordinate[j]) && (y_coordinate[i] == y_coordinate[j]))
            {
                return 1;
            }
        }
    }
    /*Store MST*/
    int NUM_EDGE = NUM_PT - 1;
    int parent[NUM_EDGE];
    int points[NUM_EDGE];
    int d[NUM_EDGE];

    for (i = 0; i < NUM_EDGE; i++)
    {
        while (fscanf(fp, "%d", &parent[i]) != 1)
        {
            if (ferror(fp))
            {
                fclose(fp);
                return 1;
            }
            if (feof(fp))
            {
                fclose(fp);
                return 1;
            }
            fscanf(fp, "%*[^\n]");
        }
        if (fscanf(fp, "%d", &points[i]) != 1)
        {
            fclose(fp);
            return 1;
        }
        if (fscanf(fp, "%d", &d[i]) != 1)
        {
            fclose(fp);
            return 1;
        }
    }
    /*create the point struct*/

    struct point p[NUM_PT]; /* Declare the following array to store the points*/
    /* indert head point to the struct array */
    /*complete the structs information*/
    for (i = 0; i < NUM_PT; i++)
    {
        p[i].index = i;
        p[i].x = x_coordinate[i];
        p[i].y = y_coordinate[i];
        p[i].parent = -1;
        /*find the parent*/
        for (j = 0; j < NUM_EDGE; j++)
        {
            if (i == points[j])
            {
                p[i].parent = parent[j];

                break;
            }
        }

        /*find the number of children and children*/
        p[i].num_children = 0;
        for (j = 0; j < NUM_EDGE; j++)
        {
            if (parent[j] == i)
            {

                p[i].child[p[i].num_children] = points[j];
                p[i].num_children++;
            }
        }
        p[i].overlap_hv = -1;
        p[i].overlap_vh = -1;
        p[i].layout = -1;
    }
    if (outputfile_pos > 0)
    {
        if ((fp = fopen(outputfile_name, "w")) == NULL)
        {
            return 1;
        }

    } else
    {
        fp = stdout;
    }

    /* open the input file and append to it*/
    FILE *infp;
    infp = fopen(inputfile_name, "a");

    /*print out the second point*/
    print_point(p, fp);

    /*dfs traverse the mst*/
    int dfs_order[NUM_PT];

    dfs_mst(p, 0, dfs_order, fp);

    /* Reverse dfs order*/
    int *reversed_dfs = reverse_dfs(dfs_order, NUM_PT);

    /* start the steiner tree*/
    int max_overlap = tree_calculate(p, reversed_dfs, NUM_PT);
    int mst_length = 0;
    float reduction_rate;

    /************** get the mst length****************/

    for (i = 0; i < NUM_EDGE; i++)
    {
        mst_length += d[i];
    }

    fprintf(fp, "\n");
    fprintf(fp, "The total overlap is %d\n", max_overlap);
    reduction_rate = (float) max_overlap / mst_length;
    fprintf(fp, "The reduction rate is %.2f\n", reduction_rate);
    /***** WRITE TO INPUT FILE****************/
    fprintf(infp, "\n");
    fprintf(infp, "#The total overlap is %d\n", max_overlap);
    fprintf(infp, "#The reduction rate is %.2f\n", reduction_rate);

    if (fp != stdout)
    {
        fclose(fp);
    }
    fclose(infp);

    free(reversed_dfs);
    free(x_coordinate);
    free(y_coordinate);
    return 0;
}

/*
 * print_point() - Print struct point to file or stdout
 */
int print_point(point *p, FILE *fp)
{

    int n = 1;
    fprintf(fp, "p[1].index = %d;\n", p[n].index);
    fprintf(fp, "p[1].x = %d;\n", p[n].x);
    fprintf(fp, "p[1].y = %d;\n", p[n].y);
    fprintf(fp, "p[1].parent = %d;\n", p[n].parent);
    fprintf(fp, "p[1].num_children = %d;\n", p[n].num_children);
    fprintf(fp, "p[1].child[8] = {");
    for (int i = 0; i < p[n].num_children; i++)
    {
        if (i == p[n].num_children - 1)
            fprintf(fp, "%d", p[n].child[i]);
        else
            fprintf(fp, "%d, ", p[n].child[i]);
    }
    fprintf(fp, "};\n");
    fprintf(fp, "p[1].overlap_hv = %d;\n", p[n].overlap_hv);
    fprintf(fp, "p[1].overlap_vh = %d;\n", p[n].overlap_vh);
    fprintf(fp, "\n");
    return 0;
}

/*
 * tree_calculate() -
 */
int tree_calculate(point *p, int * reversed_dfs, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        int index = reversed_dfs[i];
        if (p[index].num_children == 0)
        {
            /*has no children*/
            p[index].overlap_hv = 0;
            p[index].overlap_vh = 0;
        } else
        {
            /* has children*/
            int parent_layout = 1;
            global_hv_overlap = 0;
            combination(&parent_layout, p, index, p[index].num_children, p[index].num_children);
            p[index].overlap_hv = global_hv_overlap;
            parent_layout = 0;
            global_vh_overlap = 0;
            combination(&parent_layout, p, index, p[index].num_children, p[index].num_children);
            p[index].overlap_vh = global_vh_overlap;
        }
    }
    return p[0].overlap_vh;
}

/*
 * combination() - Combination
 * 0: for hv
 * 1: for vh
 */
void combination(int *parent_layout, point *p, int index, int num_children, int n)
{

    if (num_children < 1)
    {

        int *child = p[index].child;
        int overlap = overlap_calculate(parent_layout, p, index);
        for (int i = 0; i < n; i++)
        {
            if (p[child[i]].layout == 1)
            {
                overlap += p[child[i]].overlap_hv;

            } else
            {
                overlap += p[child[i]].overlap_vh;
            }
        }
        if (*parent_layout == 1)
        {
            /*hv*/
            if (global_hv_overlap == 0)
            {
                global_hv_overlap = overlap;

            } else
            {
                if (overlap > global_hv_overlap)
                {
                    global_hv_overlap = overlap;
                }

            }

        } else if (*parent_layout == 0)
        {
            /*vh*/
            if (global_vh_overlap == 0)
            {
                global_vh_overlap = overlap;
            } else
            {
                if (overlap > global_vh_overlap)
                {
                    global_vh_overlap = overlap;
                }
            }
        } else
        {
            if (global_vh_overlap == 0)
            {
                global_vh_overlap = overlap;
            } else
            {
                if (overlap > global_vh_overlap)
                {
                    global_vh_overlap = overlap;
                }
            }
            if (global_hv_overlap == 0)
            {
                global_hv_overlap = overlap;
            } else
            {
                if (overlap > global_hv_overlap)
                {
                    global_hv_overlap = overlap;
                }
            }
        }
        return;
    }
    p[p[index].child[num_children - 1]].layout = 0;
    combination(parent_layout, p, index, num_children - 1, n);
    p[p[index].child[num_children - 1]].layout = 1;
    combination(parent_layout, p, index, num_children - 1, n);
}

/*
 * overlap_calculate() -
 */
int overlap_calculate(int *parent_layout, point *p, int index)
{
    int overlap = 0;
    point root = p[index];
    int *child = p[index].child;
    int num_children = p[index].num_children;
    struct single_point turning_point[num_children];

    int max_up = 0;
    int max_down = 0;
    int max_left = 0;
    int max_right = 0;

    if (index != 0)
    {
        point parent = p[p[index].parent];
        if (if_l_shape(p, index) == 0)
        {
            /*  not L-shape */
            *parent_layout = -1;
            if (h_or_v(p, index) == 1)
            {
                /* horizontal */
                int portion = root.x - parent.x;

                if (portion > 0)
                {
                    max_left = abs(portion);
                } else
                {
                    max_right = abs(portion);
                }
            } else
            {
                /* verticall */
                int portion = root.y - parent.y;
                if (portion > 0)
                {
                    max_down = abs(portion);

                } else
                {
                    max_up = abs(portion);
                }
            }
        }

        else
        {
            /*  L-shape */
            if (*parent_layout == 1)
            {
                /* hv */
                int portion = root.y - parent.y;
                if (portion > 0)
                {
                    max_down = abs(portion);
                } else
                {
                    max_up = abs(portion);
                }
            } else
            {
                /* vh*/
                int portion = root.x - parent.x;
                if (portion > 0)
                {
                    max_left = abs(portion);
                } else
                {
                    max_right = abs(portion);
                }
            }
        }
    } else
    {
        *parent_layout = -1;
    }
    for (int i = 0; i < num_children; i++)
    {
        if (if_l_shape(p, child[i]) == 0)
        {
            if (h_or_v(p, child[i]) == 1)
            {

                p[p[index].child[i]].layout = 1;
            } else
            {
                p[p[index].child[i]].layout = 0;
            }
        }
    }

    for (int i = 0; i < num_children; i++)
    {
        /* get the turning point */

        if (p[child[i]].layout == 1)
        {
            /* hv */
            turning_point[i].x = p[child[i]].x;
            turning_point[i].y = root.y;
        } else if (p[child[i]].layout == 0)
        {
            /* vh */
            turning_point[i].x = root.x;
            turning_point[i].y = p[child[i]].y;
        }
    }
    /* for  children*/
    for (int i = 0; i < num_children; i++)
    {
        if (p[child[i]].layout == 1)
        {
            /* hv */
            int portion = turning_point[i].x - root.x;

            if (portion > 0)
            {
                /* right*/
                if (max_right == 0)
                {
                    max_right = abs(portion);
                } else
                {
                    if (max_right < abs(portion))
                    {
                        overlap += max_right;
                        max_right = abs(portion);

                    } else
                    {
                        overlap += abs(portion);
                    }
                }
            } else if (portion < 0)
            {
                /*left*/
                if (max_left == 0)
                {
                    max_left = abs(portion);
                } else
                {
                    if (max_left < abs(portion))
                    {
                        overlap += max_left;
                        max_left = abs(portion);
                    } else
                    {
                        overlap += abs(portion);
                    }
                }
            }
        } else
        {
            /* vh */
            int portion = turning_point[i].y - root.y;

            if (portion > 0)
            { //up
                if (max_up == 0)
                {
                    max_up = abs(portion);
                }

                else
                {
                    if (max_up < abs(portion))
                    {
                        overlap += max_up;
                        max_up = abs(portion);
                    } else
                    {
                        overlap += abs(portion);
                    }
                }
            } else
            { // down
                if (max_down == 0)
                {
                    max_down = abs(portion);
                } else
                {
                    if (max_down < abs(portion))
                    {
                        overlap += max_down;
                        max_down = abs(portion);
                    } else
                    {
                        overlap += abs(portion);
                    }
                }
            }
        }
    }
    return overlap;
}

/*
 * if_l_shape() -
 */
int if_l_shape(struct point *p, int current_index)
{
    if (p[current_index].x == p[p[current_index].parent].x || p[current_index].y == p[p[current_index].parent].y)
    {
        return 0;
    } else
    {
        return 1;
    }
}

/*
 * h_or_v()-
 * 1:h
 * 0:v
 * */

int h_or_v(struct point *p, int current_index)
{
    if (p[current_index].y == p[p[current_index].parent].y)
    {
        return 1;
    } else if (p[current_index].x == p[p[current_index].parent].x)
    {
        return 0;
    }
    return 0;
}

/*
 * mst_calculate() -
 */
int mst_calculate(int *x_coordinate, int *y_coordinate, int NUM_PT, int MAX_X, int MAX_Y, FILE *fp, FILE *infp)
{
    int edge[NUM_PT][NUM_PT];
    int i, j;
    for (i = 0; i < NUM_PT; i++)
    {
        for (j = i + 1; j < NUM_PT; j++)
        {
            edge[i][j] = edge[j][i] = abs(x_coordinate[i] - x_coordinate[j]) + abs(y_coordinate[i] - y_coordinate[j]);
        }
        edge[i][i] = 0;
    }

    /*Print to the file*/
    fprintf(fp, "Choosing point 0 as the root ...\n");

    for (i = 1; i < NUM_PT; i++)
    {
        fprintf(fp, "point %d has a distance %d to the tree, parent 0;\n", i, edge[i][0]);
    }
    char color[NUM_PT];
    int parent[NUM_PT];
    int d[NUM_PT];
    int num;
    int length_MST;

    /*select the first point as the root*/
    for (i = 0; i < NUM_PT; i++)
    {
        color[i] = 'w';
        d[i] = edge[i][0];
        parent[i] = 0;
    }

    color[0] = 'b';
    num = 1;
    length_MST = 0;

    int min;
    int point_added;

    /*print to the output file*/
    fprintf(fp, "#Edges of the MST by prim's algorithm:\n");

    /*append to the input file*/
    fprintf(infp, "#Edges of the MST by prim's algorithm:\n");

    while (num < NUM_PT)
    {
        min = 2 * (MAX_X + MAX_Y) + 1;
        for (i = 0; i < NUM_PT; i++)
        {
            if (color[i] == 'w' && d[i] < min)
            {
                min = d[i];
                point_added = i;

            } else if (color[i] == 'w' && d[i] == min)
            {
                if ((abs(y_coordinate[i] - y_coordinate[parent[i]]) > abs(y_coordinate[point_added] - y_coordinate[parent[point_added]]))
                        || (abs(y_coordinate[i] - y_coordinate[parent[i]]) == abs(y_coordinate[point_added] - y_coordinate[parent[point_added]])
                                && (x_coordinate[i] > x_coordinate[parent[i]] ? x_coordinate[i] : x_coordinate[parent[i]])
                                        > (x_coordinate[point_added] > x_coordinate[parent[point_added]] ?
                                                x_coordinate[point_added] : x_coordinate[parent[point_added]])))
                {
                    min = d[i];
                    point_added = i;

                }
            }

        }

        length_MST += min;
        color[point_added] = 'b';
        num++;

        fprintf(fp, "%d\t%d\t%d\n", parent[point_added], point_added, min);
        fprintf(infp, "%d\t%d\t%d\n", parent[point_added], point_added, min);
        for (i = 0; i < NUM_PT; i++)
        {
            if (color[i] == 'w' && d[i] > edge[i][point_added])
            {
                d[i] = edge[i][point_added];
                parent[i] = point_added;

            } else if (color[i] == 'w' && d[i] == edge[i][point_added])
            {
                if ((abs(y_coordinate[i] - y_coordinate[parent[i]]) < abs(y_coordinate[i] - y_coordinate[point_added]))
                        || (abs(y_coordinate[i] - y_coordinate[parent[i]]) == abs(y_coordinate[i] - y_coordinate[point_added])
                                && (x_coordinate[i] > x_coordinate[parent[i]] ? x_coordinate[i] : x_coordinate[parent[i]])
                                        < (x_coordinate[i] > x_coordinate[point_added] ? x_coordinate[i] : x_coordinate[point_added])))
                {
                    parent[i] = point_added;
                }
            }
        }

    }
    fprintf(fp, "The total length of the MST is %d\n", length_MST);
    return 0;
}

/*
 * parse_command() - Check invalid or not command.
 * Read the input file and output file from command
 */
int parse_command(int argc, char **argv, char **inputfile_name, char **outputfile_name, int *outfile_position)
{

    int inputfile_pos = 0;
    int outputfile_pos = 0;
    /* no file option */
    if (argc == 1)
        return 0;

    int i;
    for (i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-i"))
            inputfile_pos = i + 1;
        else if (!strcmp(argv[i], "-o"))
            outputfile_pos = i + 1;
    }

    if (outputfile_pos == inputfile_pos + 1)
    {
        return 1;
    }

    if (inputfile_pos == 0)
        return 1;

    *inputfile_name = argv[inputfile_pos];
    *outputfile_name = argv[outputfile_pos];
    *outfile_position = outputfile_pos;
    return 0;
}

/*
 * dfs_mst() - print the dfs order
 */
int dfs_mst(point *p, int index, int * dfs_order, FILE *fp)
{

    static int n = 0;

    fprintf(fp, "p[%d].index = %d, .num_children = %d", index, index, p[index].num_children);
    for (int i = 0; i < p[index].num_children; i++)
    {
        fprintf(fp, ", .child[%d] = %d", i, p[index].child[i]);
    }
    fprintf(fp, "\n");

    dfs_order[n] = index;
    n++;

    for (int i = 0; i < p[index].num_children; i++)
    {
        dfs_mst(p, p[index].child[i], dfs_order, fp);
    }
    return 0;
}

/*
 * reverse_dfs() - reverse the dfs order
 */
int *reverse_dfs(int* dfs_order, int n)
{
    int *reversed_dfs = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
    {
        reversed_dfs[n - i - 1] = dfs_order[i];
    }
    return reversed_dfs;
}
