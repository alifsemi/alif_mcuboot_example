/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#include <mcuboot_config/mcuboot_config.h>
#include <flash_map_backend/flash_map_backend.h>
#include <sysflash/sysflash.h>

#include <string.h>
#include <stddef.h>
#include <inttypes.h>

#include <global_map.h>

#define MRAM_SECTOR_SIZE                    1024
#define MRAM_WRITE_SIZE                     16
#define MRAM_ERASE_VALUE                    0x0
#define MRAM_ADDR_ALIGN_MASK                0xFFFFFFF0U

static struct flash_area bootloader =
{
    .fa_id = FLASH_AREA_BOOTLOADER,
    .fa_device_id = FLASH_DEVICE_MRAM,
    .fa_off = MRAM_BASE + BOOTLOADER_START_ADDRESS,
    .fa_size = BOOT_BOOTLOADER_SIZE
};

static struct flash_area primary_1 =
{
    .fa_id = FLASH_AREA_IMAGE_PRIMARY(0),
    .fa_device_id = FLASH_DEVICE_MRAM,
    .fa_off = MRAM_BASE +\
                BOOTLOADER_START_ADDRESS +\
                BOOT_BOOTLOADER_SIZE,
    .fa_size = BOOT_PRIMARY_1_SIZE
};

static struct flash_area secondary_1 =
{
    .fa_id = FLASH_AREA_IMAGE_SECONDARY(0),
    .fa_device_id = FLASH_DEVICE_MRAM,
    .fa_off = MRAM_BASE +\
                BOOTLOADER_START_ADDRESS +\
                BOOT_BOOTLOADER_SIZE +\
                BOOT_PRIMARY_1_SIZE,
    .fa_size = BOOT_SECONDARY_1_SIZE
};

#ifdef MCUBOOT_SWAP_USING_SCRATCH
static struct flash_area scratch =
{
    .fa_id = FLASH_AREA_IMAGE_SCRATCH,
    .fa_device_id = FLASH_DEVICE_MRAM,
    .fa_off = MRAM_BASE +\
               BOOTLOADER_START_ADDRESS +\
               BOOT_BOOTLOADER_SIZE +\
               BOOT_PRIMARY_1_SIZE +\
               BOOT_SECONDARY_1_SIZE,
    .fa_size = BOOT_SCRATCH_SIZE
};
#endif

struct flash_area *boot_area_descs[] =
{
    &bootloader,
    &primary_1,
    &secondary_1,
#ifdef MCUBOOT_SWAP_USING_SCRATCH
    &scratch,
#endif
    NULL
};

/**
  \fn          static void mram_write_128bit(void *dst, const uint8_t *src)
  \brief       Write 128bit data into MRAM
  \param[in]   dst  Destination MRAM address
  \param[in]   src  Source buffer address
  \return      None
*/
static void mram_write_128bit(void *dst, const void *src)
{
    /* destination (MRAM address) must be always 16-byte aligned,
     * source may or may not be aligned.*/

    /* is source data unaligned for 64-bit access? */
    if(((uint32_t)src & 0x7))
    {
        /* use temporary buffer for storing source data,
        * in case source data is not 8-bytes aligned.*/
        uint64_t temp_buf[2];

        /* unaligned source data,
         *  - copy source data first in temporary buffer
         *  - then copy buffer to destination/MRAM.
         */
        memcpy(temp_buf, src, MRAM_WRITE_SIZE);

        ((volatile uint64_t *)dst)[0] = temp_buf[0];
        ((volatile uint64_t *)dst)[1] = temp_buf[1];
    }
    else
    {
        ((volatile uint64_t *)dst)[0] = ((uint64_t *)src)[0];
        ((volatile uint64_t *)dst)[1] = ((uint64_t *)src)[1];
    }
}

/**
  \fn          static struct flash_area *get_flash_area_from_id(uint8_t id)
  \brief       Retrieve the flash area from the flash map for the given id
  \param[in]   id ID of the flash area
  \return      Reference to the flash area on success, NULL in case the area cannot be found
*/
static struct flash_area *get_flash_area_from_id(uint8_t id)
{
    struct flash_area *area = NULL;
    uint32_t i = 0;

    while (boot_area_descs[i] != NULL)
    {
        if (boot_area_descs[i]->fa_id == id)
        {
            area = boot_area_descs[i];
            break;
        }
        i++;
    }

    return area;
}

/**
  \fn          int flash_area_open(uint8_t id, const struct flash_area **area_outp)
  \brief       Retrieve the flash area from the flash map for the given id
  \param[in]   id ID of the flash area
  \param[out]  area_outp Reference to the flash area
  \return      zero on success, negative in case of an error
*/
int flash_area_open(uint8_t id, const struct flash_area **area_outp)
{
    struct flash_area *area = get_flash_area_from_id(id);

    *area_outp = area;

    return area != NULL ? 0 : -1;
}

/**
  \fn          void flash_area_close(const struct flash_area *fa)
  \brief       Close a given flash area
  \param[in]   fa Flash area
  \return      None
*/
void flash_area_close(const struct flash_area *fa)
{
    (void) fa;
    return;
}

/**
  \fn          int flash_area_read(const struct flash_area *fa, uint32_t off,
                    void *dst, uint32_t len)
  \brief       Read data from a flash area
  \param[in]   fa Flash area
  \param[in]   off Offset from the beginning of the flash area to read.
  \param[in]   dst Buffer to store the read data.
  \param[in]   len Number of bytes to read.
  \return      zero on success, negative in case of an error
*/
int flash_area_read(const struct flash_area *fa, uint32_t off,
                    void *dst, uint32_t len)
{
    uint32_t addr = fa->fa_off + off;

    memcpy(dst, (const void *) addr, len);

    return 0;
}

/**
  \fn          int flash_area_write(const struct flash_area *fa, uint32_t off,
                    void *src, uint32_t len)
  \brief       Write data to a flash area
  \param[in]   fa Flash area
  \param[in]   off Offset from the beginning of the flash area to be written.
  \param[in]   src Buffer with data to be written.
  \param[in]   len Number of bytes to read.
  \return      zero on success, negative in case of an error
*/
int flash_area_write(const struct flash_area *fa, uint32_t off,
                     const void *src, uint32_t len)
{
    uint32_t addr = fa->fa_off + off;
    const uint8_t *data = src;

    if(addr % MRAM_WRITE_SIZE) {

        // unaligned MRAM write start, write partial first
        uint8_t* ptr = (uint8_t*)(addr & MRAM_ADDR_ALIGN_MASK);
        uint8_t offset = addr & (~MRAM_ADDR_ALIGN_MASK);
        uint8_t unaligned_bytes = MRAM_WRITE_SIZE - offset;

        // read existing data in
        uint64_t temp_buf[2];

        temp_buf[0] = ((volatile uint64_t *)ptr)[0];
        temp_buf[1] = ((volatile uint64_t *)ptr)[1];

        // is unaligned bytes more than remaining count?
        if(unaligned_bytes > len)
        {
            // then take only remaining count.
            unaligned_bytes = len;
        }

        // overwrite
        memcpy((uint8_t*)temp_buf + offset, data, unaligned_bytes);

        // write back to MRAM
        mram_write_128bit(ptr, temp_buf);

        data += unaligned_bytes;
        len -= unaligned_bytes;
        addr += unaligned_bytes;
    }

    // write aligned bytes
    while(len / MRAM_WRITE_SIZE) {
        mram_write_128bit((void*)addr, data);
        len -= MRAM_WRITE_SIZE;
        data += MRAM_WRITE_SIZE;
        addr += MRAM_WRITE_SIZE;
    }

    // not full sector?
    if(len) {
        // read existing data in
        uint64_t temp_buf[2];

        temp_buf[0] = ((volatile uint64_t *)addr)[0];
        temp_buf[1] = ((volatile uint64_t *)addr)[1];

        // overwrite from the start
        memcpy(temp_buf, data, len);

        // write back to MRAM
        mram_write_128bit((void*)addr, temp_buf);
    }
    return 0;
}

/**
  \fn          int flash_area_erase(const struct flash_area *fa, uint32_t off,
                    uint32_t len)
  \brief       Erase a given flash area range
  \param[in]   fa Flash area
  \param[in]   off Offset from the beginning of the flash area to be erased..
  \param[in]   len Number of bytes to erase.
  \return      zero on success, negative in case of an error
*/
int flash_area_erase(const struct flash_area *fa,
                     uint32_t off, uint32_t len)
{
    uint32_t addr = fa->fa_off + off;
    /* buffer to act as a source of 0s for mram 'erase' */
    uint64_t src[2];
    memset(src, MRAM_ERASE_VALUE, 16);
    uint32_t i;

    if (addr % MRAM_WRITE_SIZE || len % MRAM_WRITE_SIZE)
    {
        return -1;
    }

    for (i = 0; i < len; i += MRAM_WRITE_SIZE)
    {
        mram_write_128bit((void *) (addr + i), src);
    }

    return 0;
}

/**
  \fn          uint32_t flash_area_align(const struct flash_area *fa)
  \brief       Get the write block size of a flash area
  \param[in]   fa Flash area
  \return      Write block size of the given flash area
*/
uint32_t flash_area_align(const struct flash_area *area)
{
    (void) area;

    return MRAM_WRITE_SIZE;
}

/**
  \fn          uint8_t flash_area_erased_val(const struct flash_area *area)
  \brief       Get the value expected to be read when accessing an erased flash byte
  \param[in]   fa Flash area
  \return      value of an erased flash byte
*/
uint8_t flash_area_erased_val(const struct flash_area *area)
{
    (void) area;

    return MRAM_ERASE_VALUE;
}

#ifdef MCUBOOT_USE_FLASH_AREA_GET_SECTORS
/**
  \fn          int flash_area_get_sectors(int fa_id, uint32_t *count,
                    struct flash_sector *sectors)
  \brief       Retreive information about the sectors in a given flash area
  \param[in]   fa_id ID of the flash area
  \param[in]   count Capacity of the sectors buffer.
  \param[out]  count Number of the retrieved sectors.
  \param[out]  sectors Buffer for the sector information
  \return      zero on success, negative in case of an error
*/
int flash_area_get_sectors(int fa_id, uint32_t *count,
                           struct flash_sector *sectors)
{
    struct flash_area *fa = get_flash_area_from_id((uint8_t) fa_id);

    if (fa == NULL || fa->fa_device_id != FLASH_DEVICE_MRAM)
    {
        return -1;
    }

    size_t sector_size = MRAM_SECTOR_SIZE;
    uint32_t total_count = 0;

    for (uint32_t off = 0; off < fa->fa_size; off += sector_size)
    {
        sectors[total_count].fs_size = sector_size;
        sectors[total_count].fs_off  = off;
        total_count++;
    }

    *count = total_count;

    return 0;
}
#endif

/**
  \fn          int flash_area_get_sector(const struct flash_area *area, uint32_t off,
                    struct flash_sector *sector)
  \brief       Retreive the flash sector a given offset belong to
  \param[in]   fa Flash Area
  \param[in]   off Address offset
  \param[out]  sector Buffer for the sector information
  \return      zero on success, negative in case of an error
*/
int flash_area_get_sector(const struct flash_area *area, uint32_t off,
                          struct flash_sector *sector)
{
    (void) area;

    sector->fs_off = (off / MRAM_SECTOR_SIZE) * MRAM_SECTOR_SIZE;
    sector->fs_size = MRAM_SECTOR_SIZE;

    return 0;
}

/**
  \fn          int flash_area_id_from_multi_image_slot(int image_index, int slot)
  \brief       Return the flash area ID for a given image index and slot
  \param[in]   image_index The image index
  \param[in]   slot The slot
  \return      Flash area id or negative if the requested slot is invalid
*/
int flash_area_id_from_multi_image_slot(int image_index, int slot)
{
    switch (slot)
    {
    case 0: return FLASH_AREA_IMAGE_PRIMARY(image_index);
    case 1: return FLASH_AREA_IMAGE_SECONDARY(image_index);
    case 2: return FLASH_AREA_IMAGE_SCRATCH;
    }
    return -1;
}

/**
  \fn          int flash_area_id_from_image_slot(int slot)
  \brief       Return the flash area ID for a given slot
  \param[in]   slot The slot
  \return      Flash area id or negative if the requested slot is invalid
*/
int flash_area_id_from_image_slot(int slot)
{
    return flash_area_id_from_multi_image_slot(0, slot);
}
