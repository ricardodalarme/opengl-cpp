/**
 * @file cgImage.h
 * @brief Declaration of image functions.
 * @author Ricardo Dutra da Silva
 */


#ifndef _CGIMAGE_H_
#define _CGIMAGE_H_


/* Includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


/* Defines. */
#define CG_IMAGE_TYPE_UNKNOWN   0 
#define CG_IMAGE_TYPE_PGM_ASCII 1 
#define CG_IMAGE_TYPE_PGM_RAW   2 

#define CG_FALSE 0
#define CG_TRUE  1


/* Types. */

/// cgMat2i
/** The struct represents an integer two dimensional matrix. 
 */
typedef struct cg_mat_2i
{
    /// Number of rows.
    /** The number of rows of the matrix. */
    int height;
    /// Number of columns.
    /** The number of columns of the matrix. */
    int width;
    /// Matrix.
    /** The integer matrix. */
    int **val;

} *cgMat2i;

/* Functions. */

/// Allocate integer 2D matrix.
/**
 * This function allocates an integer two dimensional matrix. 
 * @param height number of columns.
 * @param width number of columns.
 * @return matrix or NULL (in case of error).
 */
cgMat2i cgAllocateMat2i(
    int height,
    int width
);

/// Free integer 2D matrix.
/**
 * This function frees an integer two dimensional matrix.
 * @param mat matrix to be freed.
 */
void cgFreeMat2i(
    cgMat2i mat
);

/// Maximum value.
/**
 * This function retuns the maximum value of an integer matrix. 
 * @param mat matrix.
 * @return maximum value.
 */
int cgMatMaxValue2i(
    cgMat2i mat
);

/// Minimum value.
/**
 * This function retuns the minimum value of an integer matrix. 
 * @param mat matrix.
 * @return maximum value.
 */
int cgMatMinValue2i(
    cgMat2i mat
);

/// Read PGM image.
/**
 * This function reads an image from a PGM file.
 * @param fname pgm file name.
 * @return gray-tone image or NULL in error.
 */
cgMat2i cgReadPGMImage(
    const char *fname
);

/// Write PGM image.
/**
 * This function writes an image to a PGM file.
 * @param img image to write.
 * @param fname pgm file name.
 * @param type image type.
 */
void cgWritePGMImage(
    cgMat2i img,
    char *fname,
    int type
);

/// Parse PGM header.
/**
 * This function reads all information in PGM file header.
 * @param fp File pointer.
 * @param nr Return number of rows.
 * @param nc Return number of columns.
 * @param mv Return maximum value.
 * @param type Return the type of the file (CG_IMAGE_TYPE_PGM_ASCII,
 * CG_IMAGE_TYPE_PGM_RAW, VG_IMAGE_TYPE_UNKNOWN).
 * @return CG_TRUE if successfull; CG_FALSE otherwise.
 *
 */
int ParsePGMHeader(
    FILE *fp,
    int *nr,
    int *nc,
    int *mv,
    int *type
);

/// Read PGM file white spaces.
/**
 * This function reads whitespaces (blanks, TABs, CRs, LFs) from a PGM file.
 * @param fp file pointer.
 */
void ReadWhiteSpace(
    FILE *fp
);
	
/// Read PGM file comments.
/**
 * This function reads comments from a PGM file. These are lines
 * starting with the '#' character.
 * @param fp file pointer.
 */
void ReadComment(
    FILE *fp
);

/// Print error message.
/**
 * This function prints an error message to the standard output device.
 * @param func function name (without period).
 * @param msg error message (with period).
 */
void cgError(
    const char *func,
    const char *msg
);

#endif /* _CGIMAGE_H_ */
