/**
 * @file cgImage.c
 * @brief Implementation of the gray image functions.
 * @author Ricardo Dutra da Silva
 */


#include "cgImage.h"

cgMat2i cgAllocateMat2i(
    int height,
    int width
)
{
    int r;

    /* Allocate structure. */
    cgMat2i mat = (cgMat2i) malloc(sizeof(struct cg_mat_2i));

    if (mat == NULL)
    {
        cgError("cgAllocateMat2i", "No memory available.");
        return NULL;
    }

    /* Set properties. */
    mat->height = height;
    mat->width  = width;

    /* Allocate rows. */
    mat->val = (int**) malloc(height*sizeof(int*));

    if (mat->val == NULL)
    {
        cgError("cgAllocateMat2i", "No memory available.");
        return NULL;
    }

    /* Allocate columns for each row. */
    for (r = 0; r < height; r++)
    {
        mat->val[r] = (int*) malloc(width*sizeof(int));
        
        if (mat->val[r] == NULL)
        {
            cgError("cgAllocateMat2i", "No memory available.");
            return NULL;
        }

        memset(mat->val[r], 0, width*sizeof(int));
    }

    return mat;
}

int cgMatMinValue2i(
    cgMat2i mat
)
{
    int min; 
    int r, c;

    min = INT_MAX;
    for (r = 0; r < mat->height; r++)
        for (c = 0; c < mat->width; c++)
            if (mat->val[r][c] < min)
                min = mat->val[r][c];

    return min;
}

int cgMatMaxValue2i(
    cgMat2i mat
)
{
    int max; 
    int r, c;

    max = -INT_MAX;
    for (r = 0; r < mat->height; r++)
        for (c = 0; c < mat->width; c++)
            if (mat->val[r][c] > max)
                max = mat->val[r][c];

    return max;
}

cgMat2i cgReadPGMImage(
    const char *fname
)
{
    int r, c;
    unsigned char b, b1, b2;
    unsigned short bb;

    /* Open file. */
    FILE *fp = fopen(fname, "r");

    if (fp == NULL)
    {
	char *str = NULL;
        sprintf(str, "Unable to open file %s", fname);
        cgError("cgReadPGMImage", str); 
        return NULL;
    }

    /* Parse Header. */
    int nr, nc, mv, type;

    if (ParsePGMHeader(fp, &nr, &nc, &mv, &type) == CG_FALSE)
    {
        cgError("cgReadPGMImage", "Invalid header."); 
        fclose(fp);
        return NULL;
    }
    
    if (mv > 65535)
    {
        cgError("cgReadPGMImage", "Invalid maximum value."); 
        fclose(fp);
        return NULL;
    }

    if (type == CG_IMAGE_TYPE_UNKNOWN)
    {
        cgError("cgReadPGMImage", "Invalid file type."); 
        fclose(fp);
        return NULL;
    }

    /* Create image. */
    cgMat2i img = cgAllocateMat2i(nr,nc);
    if (img == NULL)
    {
        cgError("cgReadPGMImage", "Creating image."); 
        fclose(fp);
        return NULL;
    }

    /* Read pixels. */
    if (type == CG_IMAGE_TYPE_PGM_ASCII)
    {
        for (r = 0; r < nr; r++)
        {
            for (c = 0; c < nc; c++)
            {
                if (fscanf(fp, "%d", &img->val[r][c]) == EOF)
                {
                    cgError("cgReadPGMImage", "File ended prematurely."); 
                    fclose(fp);
                    return img;
                }
            }
        }
    }
    else if (type == CG_IMAGE_TYPE_PGM_RAW)
    {
        for (r = 0; r < nr; r++)
        {
            for (c = 0; c < nc; c++)
            {
                if (mv < 256)
                {
                    if (fread(&b, 1, 1, fp) == 0)
                    {
                        cgError("cgReadPGMImage", "File ended prematurely."); 
                        fclose(fp);
                        return img;
                    }
                    img->val[r][c] = (int)b;
                }
                else
                {       
                    if (fread(&b1, 1, 1, fp) == 0)
                    {
                        cgError("cgReadPGMImage", "File ended prematurely."); 
                        fclose(fp);
                        return img;
                    }
                    
                    if (fread(&b2, 1, 1, fp) == 0)
                    {
                        cgError("cgReadPGMImage", "File ended prematurely."); 
                        fclose(fp);
                        return img;
                    }
                    bb = 0;
                    bb = (bb | b1) << 8;
                    bb = (bb | b2);
                    img->val[r][c] = (int) bb;
                }
            }
        }
    }

    /* Close file. */
    fclose(fp);

    return img;
}

void cgWritePGMImage(
    cgMat2i img,
    char *fname,
    int type
)
{
    int r, c;
    int b, mv;
    int vpl;
    char str[32] = "";

    /* Check input. */
    if (img == NULL)
    {
        cgError("cgWritePGMImage", "NULL image."); 
        return;
    }

    /* Open file. */
    FILE *fp = fopen(fname, "w");

    if (fp == NULL)
    {
        sprintf(str, "Unable to open file %s", fname);
        cgError("cgWritePGMImage", str); 
        return;
    }

    /* Write header. */

    /* Magic number.*/
    if (type == CG_IMAGE_TYPE_PGM_ASCII)
        fprintf(fp, "P2\n");
    else if (type == CG_IMAGE_TYPE_PGM_RAW)
        fprintf(fp, "P5\n");
    else
    {
        cgError("cgWritePGMImage", "Invalid image type."); 
        return;
    }

    /* Width and height. */
    fprintf(fp, "%d %d\n", img->width, img->height);

    /* Maximum gray value. */
    mv =  cgMatMaxValue2i(img);
    fprintf(fp, "%d\n", mv);

    /* Write pixels. */
    if (type == CG_IMAGE_TYPE_PGM_ASCII)
    {
        vpl = 0;
        for (r = 0; r < img->height; r++)
        {
            for (c = 0; c < img->width; c++)
            {
                if (vpl <= 18){
                    fprintf(fp, "%d ", (int)img->val[r][c]);
                    vpl++;
                }
                else
                {
                    fprintf(fp, "%d\n", (int)img->val[r][c]);
                    vpl = 0;
                }
            }
        }
    }
    else if (type == CG_IMAGE_TYPE_PGM_RAW)
    {
        for (r = 0; r < img->height; r++)
        {
            for (c = 0; c < img->width; c++)
            {
                if (mv < 256)
                {
                    b = img->val[r][c];
                    fwrite(&b, 1, 1, fp);
                }
                else
                {
                    b = ((int)img->val[r][c] & 0x0000ff00) >> 8;
                    fwrite(&b, 1, 1, fp);
                    b = (int)img->val[r][c] & 0x000000ff;
                    fwrite(&b, 1, 1, fp);
                }
            }
        }
    }

    /* Close file. */
    fclose(fp);
}

int ParsePGMHeader(
    FILE *fp,
    int *nr,
    int *nc,
    int *mv,
    int *type
)
{
    /* Init type as unknown. */
    *type = CG_IMAGE_TYPE_UNKNOWN;

    /* Get PGM magic number. */
    int id1 = fgetc(fp);
    while ((id1 != 'P'))
    {
        switch (id1)
        {
            case  ' ':
            case '\t':
            case '\n':
            case '\r':
                ReadWhiteSpace(fp);
                break;
            case '#':
                ReadComment(fp);
                break;
            default:
                cgError("ParsePGMHeader", "Unknown file type."); 
                return CG_FALSE;
        }
        id1 = fgetc(fp);
    }
    
    int id2 = fgetc(fp);
    if (id2 == '2')
    {
        *type = CG_IMAGE_TYPE_PGM_ASCII;
    }
    else if (id2 == '5')
    {
        *type = CG_IMAGE_TYPE_PGM_RAW;
    }
    else
    {
        cgError("ParsePGMHeader", "Unknown file type."); 
        return CG_FALSE;
    }

    /* Read image width. */
    int c = fgetc(fp);
    while ((c < '0') || (c > '9'))
    {
        switch (c)
        {
            case  ' ':
            case '\t':
            case '\n':
            case '\r':
                ReadWhiteSpace(fp);
                break;
            case '#':
                ReadComment(fp);
                break;
            default:
                cgError("ParsePGMHeader", "Incorrect width."); 
                return CG_FALSE;
        }
        c = fgetc(fp);
    }
    ungetc(c, fp);
    
    int fret = fscanf(fp, "%d\n", nc);
    if ((fret == 0) || (fret == EOF))
    {
        cgError("ParsePGMHeader", "Failed reading header."); 
        return CG_FALSE;
    }
    
    /* Read image height. */
    c = fgetc(fp);
    while ((c < '0') || (c > '9'))
    {
        switch (c)
        {
            case  ' ':
            case '\t':
            case '\n':
            case '\r':
                ReadWhiteSpace(fp);
                break;
            case '#':
                ReadComment(fp);
                break;
            default:
                cgError("ParsePGMHeader", "Incorrect height."); 
                return CG_FALSE;
        }
        c = fgetc(fp);
    }
    ungetc(c, fp);

    fret = fscanf(fp, "%d\n", nr);
    if ((fret == 0) || (fret == EOF))
    {
        cgError("ParsePGMHeader", "Failed reading header."); 
        return CG_FALSE;
    }
    
    /* Read image maximum value. */
    c = fgetc(fp);
    while ((c < '0') || (c > '9'))
    {
        switch (c)
        {
            case  ' ':
            case '\t':
            case '\n':
            case '\r':
                ReadWhiteSpace(fp);
                break;
            case '#':
                ReadComment(fp);
                break;
            default:
                cgError("ParsePGMHeader", "Incorrect maximum value."); 
                return CG_FALSE;
        }
        c = fgetc(fp);
    }
    ungetc(c, fp);

    fret = fscanf(fp, "%d\n", mv);
    if ((fret == 0) || (fret == EOF))
    {
        cgError("ParsePGMHeader", "Failed reading header."); 
        return CG_FALSE;
    }

    return CG_TRUE;
}

void ReadWhiteSpace(FILE *fp)
{
    int c;
    int white_space = 1;

    while (white_space)
    {
        c = fgetc(fp);
        if ((c != ' ') || (c != '\t') || (c != '\n') || (c != '\r'))
        {
            white_space = 0; 
        }
    }

    ungetc(c, fp);
}

void ReadComment(FILE *fp)
{
    int c;
    int comment = 1;

    while (comment)
    {
        c = fgetc(fp);
        if ((c == '\n') || (c == '\r'))
        {
            comment = 0; 
        }
    }
}

void cgFreeMat2i(
    cgMat2i mat
)
{
    int r;

    /* Check if mat is not NULL. */
    if (mat == NULL)
    {
        cgError("cgFreeMat2i", "Matrix is NULL.");
        return;
    }
    
    /* Free the columns of each row. */
    for (r = 0; r < mat->height; r++)
    {
        free(mat->val[r]);
    }

    /* Free rows. */
    free(mat->val);

    /* Free matrix. */
    free(mat);
}

void cgError(
    const char *func,
    const char *msg
)
{
    if ((func != NULL) && (msg != NULL))
        fprintf(stdout, "\n%s:%s\n", func, msg);
    else if ((func != NULL) && (msg == NULL))
        fprintf(stdout, "\n%s:Unknown.\n", func);
    else if ((func == NULL) && (msg != NULL))
        fprintf(stdout, "\nUnknown:%s\n", msg);
    else if ((func == NULL) && (msg == NULL))
        fprintf(stdout, "\nUnknown:Unknown.\n");
}
