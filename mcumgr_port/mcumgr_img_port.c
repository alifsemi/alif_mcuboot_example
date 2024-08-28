#include "bootutil/bootutil_public.h"
#include "mgmt/mgmt.h"
#include "img_mgmt/img_mgmt.h"
#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"

#define MRAM_WRITE_SIZE 16

int img_mgmt_impl_read(int slot, unsigned int offset, void *dst,
                   unsigned int num_bytes)
{
    const struct flash_area *fa;
    int area_id;
    int rc;

    area_id = flash_area_id_from_image_slot(slot);
    rc = flash_area_open(area_id, &fa);
    if (rc != 0) {
        return MGMT_ERR_EUNKNOWN;
    }

    rc = flash_area_read(fa, offset, dst, num_bytes);
    flash_area_close(fa);
    if (rc != 0) {
        return MGMT_ERR_EUNKNOWN;
    }

    return 0;
}

int img_mgmt_impl_erased_val(int slot, uint8_t *erased_val)
{
    const struct flash_area *fa;
    int rc;

    rc = flash_area_open(flash_area_id_from_image_slot(slot), &fa);
    if (rc != 0) {
      return MGMT_ERR_EUNKNOWN;
    }

    *erased_val = flash_area_erased_val(fa);
    flash_area_close(fa);

    return 0;
}

int img_mgmt_impl_swap_type(int slot)
{
    assert(slot == 0 || slot == 1);

    switch (boot_swap_type()) {
    case BOOT_SWAP_TYPE_NONE:
        return IMG_MGMT_SWAP_TYPE_NONE;
    case BOOT_SWAP_TYPE_TEST:
        return IMG_MGMT_SWAP_TYPE_TEST;
    case BOOT_SWAP_TYPE_PERM:
        return IMG_MGMT_SWAP_TYPE_PERM;
    case BOOT_SWAP_TYPE_REVERT:
        return IMG_MGMT_SWAP_TYPE_REVERT;
    default:
        assert(0);
        return IMG_MGMT_SWAP_TYPE_NONE;
    }
}

int img_mgmt_impl_erase_slot(void)
{
    const struct flash_area *fa;
    int rc = flash_area_open(FLASH_AREA_IMAGE_0_SECONDARY, &fa);
    if (rc) {
        return MGMT_ERR_EUNKNOWN;
    }

    uint8_t erasedata[MRAM_WRITE_SIZE];
    memset(erasedata, flash_area_erased_val(fa), MRAM_WRITE_SIZE);

    for(uint32_t i = 0; i <= (fa->fa_size - MRAM_WRITE_SIZE) && !rc; i += MRAM_WRITE_SIZE) {
        rc = flash_area_write(fa, i, erasedata, MRAM_WRITE_SIZE);
    }
    flash_area_close(fa);

    if (rc != 0) {
        return MGMT_ERR_EUNKNOWN;
    }

    return 0;
}

int img_mgmt_impl_erase_image_data(unsigned int off, unsigned int num_bytes)
{
    (void)off;
    (void)num_bytes;
    return 0;
}

int img_mgmt_impl_write_image_data(unsigned int offset, const void *data,
                               unsigned int num_bytes, bool last)
{
    (void)last;
    const struct flash_area *fa;
    int rc;

    rc = flash_area_open(FLASH_AREA_IMAGE_0_SECONDARY, &fa);
    if (rc != 0) {
        return MGMT_ERR_EUNKNOWN;
    }

    rc = flash_area_write(fa, offset, data, num_bytes);
    flash_area_close(fa);
    if (rc != 0) {
        return MGMT_ERR_EUNKNOWN;
    }

    return 0;
}

int img_mgmt_impl_write_confirmed(void)
{
    int rc;

    rc = boot_set_confirmed_multi(0);
    if (rc != 0) {
        return MGMT_ERR_EUNKNOWN;
    }

    return 0;
}

int img_mgmt_impl_write_pending(int slot, bool permanent)
{
    (void)slot;
    int err = boot_set_pending_multi(0, permanent);
    if(err) {
        return MGMT_ERR_EUNKNOWN;
    }
    return 0;
}

int img_mgmt_impl_upload_inspect(const struct img_mgmt_upload_req *req,
                                 struct img_mgmt_upload_action *action,
                                 const char **errstr)
{
    const struct image_header *hdr;
    
    memset(action, 0, sizeof *action);

    if (req->off == ((unsigned long long int)0 - 1)) {
        /* Request did not include an `off` field. */
        *errstr = img_mgmt_err_str_hdr_malformed;
        return MGMT_ERR_EINVAL;
    }

    if (req->off == 0) {
        /* First upload chunk. */
        if (req->data_len < sizeof(struct image_header)) {
            /*
             * Image header is the first thing in the image.
             */
            *errstr = img_mgmt_err_str_hdr_malformed;
            return MGMT_ERR_EINVAL;
        }

        if (req->size == ((unsigned long long int)0 - 1)) {
            /* Request did not include a `len` field. */
            *errstr = img_mgmt_err_str_hdr_malformed;
            return MGMT_ERR_EINVAL;
        }
        action->size = req->size;

        hdr = (struct image_header *)req->img_data;
        if (hdr->ih_magic != IMAGE_MAGIC) {
            *errstr = img_mgmt_err_str_magic_mismatch;
            return MGMT_ERR_EINVAL;
        }

        if (req->data_sha_len > IMG_MGMT_DATA_SHA_LEN) {
            return MGMT_ERR_EINVAL;
        }

        action->area_id = 0;

        if (req->upgrade) {
            // request for upgrade only.
            // TODO: check running version and compare.
        }
    }
    else {
        /* Continuation of upload. */
        action->area_id = g_img_mgmt_state.area_id;
        action->size = g_img_mgmt_state.size;

        if (req->off != g_img_mgmt_state.off) {
            /*
             * Invalid offset. Drop the data, and respond with the offset we're
             * expecting data for.
             */
            return 0;
        }
    }

    // never erase while MRAM is the only backend.
    action->erase = false;

    action->write_bytes = req->data_len;

    action->proceed = true;

    return 0;
}

int img_mgmt_impl_log_upload_start(int status)
{
    (void)status;
    return 0;
}

int img_mgmt_impl_log_upload_done(int status, const uint8_t *hash)
{
    (void)status;
    (void)hash;
    return 0;
}

int img_mgmt_impl_log_pending(int status, const uint8_t *hash)
{
    (void)status;
    (void)hash;
    return 0;
}

int img_mgmt_impl_log_confirm(int status, const uint8_t *hash)
{
    (void)status;
    (void)hash;
    return 0;
}
