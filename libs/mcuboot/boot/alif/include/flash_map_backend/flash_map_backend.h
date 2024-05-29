/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef _FLASH_MAP_BACKEND_H_
#define _FLASH_MAP_BACKEND_H_
#include <inttypes.h>

/** \brief Representation of a flash area */
struct flash_area {
  uint8_t  fa_id;         /**< Flash area identifier*/
  uint8_t  fa_device_id;  /**< The ID of the device the flash area resides on */
  uint16_t pad16;
  uint32_t fa_off;        /**< The flash offset from the beginning */
  uint32_t fa_size;       /**< The size of this flash area */
};

/** \brief Representation of a flash sector */
struct flash_sector {
  uint32_t fs_off; /**< Offset of this sector, from the start of its flash area (not device). */
  uint32_t fs_size; /**< Size of this sector, in bytes. */
};

/**
  \fn          static inline uint8_t flash_area_get_device_id(const struct flash_area *fa)
  \brief       Return the ID of the device in which a given flash area resides on.
  \param[in]   Flash area
  \return      The device ID of the requested flash area.
*/
static inline uint8_t flash_area_get_device_id(const struct flash_area *fa)
{
    return fa->fa_device_id;
}

/**
  \fn          static inline uint32_t flash_area_get_off(const struct flash_area *fa)
  \brief       Return the offset of a flash area from the beginning of the device..
  \param[in]   Flash area
  \return      The offset of the requested flash area.
*/
static inline uint32_t flash_area_get_off(const struct flash_area *fa)
{
	return fa->fa_off;
}

/**
  \fn          static inline uint32_t flash_area_get_size(const struct flash_area *fa)
  \brief       Return the size of a flash area.
  \param[in]   Flash area
  \return      The size of the requested flash area.
*/
static inline uint32_t flash_area_get_size(const struct flash_area *fa)
{
	return fa->fa_size;
}

/**
  \fn          static inline uint8_t flash_area_get_id(const struct flash_area *fa)
  \brief       Return the ID of a flash area.
  \param[in]   Flash area
  \return      The ID of the requested flash area.
*/
static inline uint8_t flash_area_get_id(const struct flash_area *fa)
{
	return fa->fa_id;
}

/**
  \fn          static inline uint32_t flash_sector_get_off(const struct flash_sector *fs)
  \brief       Return the offset of a given flash sector
  \param[in]   fs Flash sector
  \return      The offset of the requested flash sector.
*/
static inline uint32_t flash_sector_get_off(const struct flash_sector *fs)
{
	return fs->fs_off;
}

/**
  \fn          static inline uint32_t flash_sector_get_size(const struct flash_sector *fs)
  \brief       Return the size of a given flash sector
  \param[in]   fs Flash sector
  \return      The size of the requested flash sector.
*/
static inline uint32_t flash_sector_get_size(const struct flash_sector *fs)
{
	return fs->fs_size;
}

/**
  \fn          int flash_area_open(uint8_t id, const struct flash_area **area_outp)
  \brief       Retrieve the flash area from the flash map for the given id
  \param[in]   id ID of the flash area
  \param[out]  area_outp Reference to the flash area
  \return      zero on success, negative in case of an error
*/
int flash_area_open(uint8_t id, const struct flash_area **area_outp);

/**
  \fn          void flash_area_close(const struct flash_area *fa)
  \brief       Close a given flash area
  \param[in]   fa Flash area
  \return      None
*/
void flash_area_close(const struct flash_area *fa);

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
                    void *dst, uint32_t len);

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
                     const void *src, uint32_t len);
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
                     uint32_t off, uint32_t len);
/**
  \fn          uint32_t flash_area_align(const struct flash_area *fa)
  \brief       Get the write block size of a flash area
  \param[in]   fa Flash area
  \return      Write block size of the given flash area
*/
uint32_t flash_area_align(const struct flash_area *area);

/**
  \fn          uint8_t flash_area_erased_val(const struct flash_area *area)
  \brief       Get the value expected to be read when accessing an erased flash byte
  \param[in]   fa Flash area
  \return      value of an erased flash byte
*/
uint8_t flash_area_erased_val(const struct flash_area *area);

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
                           struct flash_sector *sectors);

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
                          struct flash_sector *sector);

/**
  \fn          int flash_area_id_from_multi_image_slot(int image_index, int slot)
  \brief       Return the flash area ID for a given image index and slot
  \param[in]   image_index The image index
  \param[in]   slot The slot
  \return      Flash area id or negative if the requested slot is invalid
*/
int flash_area_id_from_multi_image_slot(int image_index, int slot);

/**
  \fn          int flash_area_id_from_image_slot(int slot)
  \brief       Return the flash area ID for a given slot
  \param[in]   slot The slot
  \return      Flash area id or negative if the requested slot is invalid
*/
int flash_area_id_from_image_slot(int slot);
#endif /* _FLASH_MAP_BACKEND_H_ */
