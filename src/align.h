/**
 * Copyright(C) 2018, Juniper Networks, Inc.
 * All rights reserved
 *
 * This SOFTWARE is licensed under the license provided in the LICENSE.txt
 * file.  By downloading, installing, copying, or otherwise using the
 * SOFTWARE, you agree to be bound by the terms of that license.  This
 * SOFTWARE is not an official Juniper product.
 *
 * Third-Party Code: This SOFTWARE may depend on other components under
 * separate copyright notice and license terms.  Your use of the source
 * code for those components is subject to the term and conditions of
 * the respective license as noted in the Third-Party source code.
 */

/**
 * @author Anoop Saldanha
 */

#ifndef __ALIGN__H__
#define __ALIGN__H__

#define ALIGN_UP(offset, alignment) do { \
        (offset) = ((offset) + (alignment) - 1) & ~((alignment) - 1); \
    } while (0)

#define ALIGN_DOWN(offset, alignment) do { \
        (offset) = ((offset))  & ~((alignment) - 1);   \
    } while (0)

#endif /* __ALIGN__H__ */
