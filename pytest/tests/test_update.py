_IMAGES_HE_UPDATE = "alif-example-app-update_signed.bin"
_IMAGES_HE_ORIGINAL = "alif-example-app_as_update_signed.bin"
_IMAGES_HP_UPDATE = "alif-blinky-app-update_signed.bin"
_IMAGES_HP_ORIGINAL = "alif-blinky-app_as_update_signed.bin"
_IMAGES_SERAM_103 = "seram_1.103.0_signed.bin"
_IMAGES_SERAM_104 = "seram_1.104.0_signed.bin"


def test_startup_prints(SE, HE, HP):
    SE.send_isp_reset()
    HE.expect("Loading image, version 1.0.0")
    HP.expect("Loading image, version 1.0.0")
    HE.expect("Example app running!")
    HP.expect("Blink start")


def test_mcumgr_echo(HE, MGRCLI):
    MGRCLI.command_and_assert("echo Boo", "Boo")
    HE.expect("Handled MCUMGR command, group: 0 id: 0 result code: 0")


def test_mcumgr_list_images(HE, MGRCLI):
    resp = MGRCLI.command("image list")
    resp.assert_doesnt_contain("image=0 slot=1")
    resp.assert_doesnt_contain("image=1 slot=1")
    resp.assert_contains("Images")
    resp.assert_contains("image=0 slot=0")
    resp.assert_contains("version: 1.0.0")
    resp.assert_contains("image=1 slot=0")
    resp.assert_contains("version: 1.0.0")
    HE.expect("Handled MCUMGR command, group: 1 id: 0 result code: 0")


def test_mcumgr_reset(HE, MGRCLI):
    MGRCLI.command_and_assert("reset", "Done")
    HE.expect("Loading image, version 1.0.0")
    HE.expect("Example app running!")


def test_mcumgr_upload_erase(HE, MGRCLI):
    MGRCLI.command_image_upload(_IMAGES_HE_UPDATE)
    MGRCLI.command_image_upload(_IMAGES_HP_UPDATE, image_id=1)
    resp = MGRCLI.command("image list")
    resp.assert_contains("Images")
    resp.assert_contains("image=0 slot=0")
    resp.assert_contains("version: 1.0.0")
    resp.assert_contains("image=0 slot=1")
    resp.assert_contains("version: 2.0.0")
    resp.assert_contains("image=1 slot=0")
    resp.assert_contains("version: 1.0.0")
    resp.assert_contains("image=1 slot=1")
    resp.assert_contains("version: 2.0.0")
    HE.expect("Handled MCUMGR command, group: 1 id: 0 result code: 0")
    MGRCLI.command_and_assert("image erase", "Done")
    resp = MGRCLI.command("image list")
    resp.assert_doesnt_contain("image=0 slot=1")
    resp.assert_doesnt_contain("image=1 slot=1")
    resp.assert_contains("Images")
    resp.assert_contains("image=0 slot=0")
    resp.assert_contains("version: 1.0.0")
    resp.assert_contains("image=1 slot=0")
    resp.assert_contains("version: 1.0.0")


def test_mcumgr_upload_update_he_revert(SE, HE, MGRCLI):
    MGRCLI.command_image_upload(_IMAGES_HE_UPDATE)
    resp = MGRCLI.command("image list")
    resp.assert_contains("Images")
    resp.assert_contains("image=0 slot=0")
    resp.assert_contains("version: 1.0.0")
    resp.assert_contains("image=0 slot=1")
    resp.assert_contains("version: 2.0.0")
    hash = resp.assert_get_regx("hash: ([a-f0-9]*)")
    resp.assert_contains("image=1 slot=0")
    resp.assert_contains("version: 1.0.0")
    HE.expect("Handled MCUMGR command, group: 1 id: 0 result code: 0")
    MGRCLI.command("image test %s" % hash)
    SE.send_isp_reset()
    HE.expect("Loading image, version 2.0.0")
    SE.send_isp_reset()
    HE.expect("Loading image, version 1.0.0")
    MGRCLI.command_and_assert("image erase", "Done")


def test_mcumgr_upload_update_he_confirm(SE, HE, MGRCLI):
    MGRCLI.command_image_upload(_IMAGES_HE_UPDATE)
    resp = MGRCLI.command("image list")
    resp.assert_contains("Images")
    resp.assert_contains("image=0 slot=0")
    resp.assert_contains("version: 1.0.0")
    resp.assert_contains("image=0 slot=1")
    resp.assert_contains("version: 2.0.0")
    hash = resp.assert_get_regx("hash: ([a-f0-9]*)")
    resp.assert_contains("image=1 slot=0")
    resp.assert_contains("version: 1.0.0")
    HE.expect("Handled MCUMGR command, group: 1 id: 0 result code: 0")
    MGRCLI.command("image test %s" % hash)
    SE.send_isp_reset()
    HE.expect("Loading image, version 2.0.0")
    MGRCLI.command("image confirm %s" % hash)
    SE.send_isp_reset()
    HE.expect("Loading image, version 2.0.0")
    MGRCLI.command_and_assert("image erase", "Done")
    MGRCLI.command_image_upload(_IMAGES_HE_ORIGINAL)
    resp = MGRCLI.command("image list")
    resp.assert_contains("Images")
    resp.assert_contains("image=0 slot=0")
    resp.assert_contains("version: 2.0.0")
    resp.assert_contains("image=0 slot=1")
    resp.assert_contains("version: 1.0.0")
    hash = resp.assert_get_regx("hash: ([a-f0-9]*)")
    MGRCLI.command("image confirm %s" % hash)
    SE.send_isp_reset()
    HE.expect("Example app running!")
    MGRCLI.command_and_assert("image erase", "Done")
    resp = MGRCLI.command("image list")
    resp.assert_contains("Images")
    resp.assert_contains("image=0 slot=0")
    resp.assert_contains("version: 1.0.0")


def test_mcumgr_upload_update_hp(SE, HE, HP, MGRCLI):
    MGRCLI.command_image_upload(_IMAGES_HP_UPDATE, image_id=1)
    resp = MGRCLI.command("image list")
    resp.assert_contains("Images")
    resp.assert_contains("image=1 slot=0")
    resp.assert_contains("version: 1.0.0")
    resp.assert_contains("image=1 slot=1")
    resp.assert_contains("version: 2.0.0")
    hash = resp.assert_get_regx("hash: ([a-f0-9]*)")
    HE.expect("Handled MCUMGR command, group: 1 id: 0 result code: 0")
    MGRCLI.command("image test %s" % hash)
    SE.send_isp_reset()
    HP.expect("Loading image, version 2.0.0")
    HE.expect("Example app running!")
    SE.send_isp_reset()
    HP.expect("Loading image, version 2.0.0")
    HE.expect("Example app running!")
    MGRCLI.command_and_assert("image erase", "Done", tries=2)
    MGRCLI.command_image_upload(_IMAGES_HP_ORIGINAL, image_id=1)
    resp = MGRCLI.command("image list")
    resp.assert_contains("Images")
    resp.assert_contains("image=1 slot=0")
    resp.assert_contains("version: 2.0.0")
    resp.assert_contains("image=1 slot=1")
    resp.assert_contains("version: 1.0.0")
    hash = resp.assert_get_regx("hash: ([a-f0-9]*)")
    MGRCLI.command("image test %s" % hash)
    SE.send_isp_reset()
    HP.expect("Loading image, version 1.0.0")
    HE.expect("Example app running!")
    MGRCLI.command_and_assert("image erase", "Done", tries=2)
    resp = MGRCLI.command("image list")
    resp.assert_contains("Images")
    resp.assert_contains("image=1 slot=0")
    resp.assert_contains("version: 1.0.0")

def test_mcumgr_seram_update_103(SE, HE, MGRCLI, SERAM_UPDATE):
    # Reset SoC if we happen to have update pending
    SE.send_isp_reset()

    # Reset yet again as we are now running from SERAM1 if update was pending
    SE.send_isp_reset()

    # Now we should be running from SERAM0 and all house keeping should be done
    SE.send_isp_reset()

    MGRCLI.command_image_upload(_IMAGES_SERAM_103, image_id=2)

    resp = MGRCLI.command("image list")
    resp.assert_contains("Images")
    resp.assert_contains("image=2 slot=1")
    resp.assert_contains("version: 1.103.0")
    hash = resp.assert_get_regx("hash: ([a-f0-9]*)")
    MGRCLI.command("image test %s" % hash)

    SE.send_isp_reset()
    HE.expect("Updating SE image")
    HE.expect("Example app running!", timeout=30)

    SE.send_isp_reset()
    HE.expect("Current SE version: 1.103.0")
    HE.expect("Example app running!")

    SE.send_isp_reset()
    HE.expect("Current SE version: 1.103.0")
    HE.expect("Example app running!")

def test_mcumgr_seram_update_104(SE, HE, MGRCLI, SERAM_UPDATE):
    # Reset SoC if we happen to have update pending
    SE.send_isp_reset()

    # Reset yet again as we are now running from SERAM1 if update was pending
    SE.send_isp_reset()

    # Now we should be running from SERAM0 and all house keeping should be done
    SE.send_isp_reset()

    MGRCLI.command_image_upload(_IMAGES_SERAM_104, image_id=2)

    resp = MGRCLI.command("image list")
    resp.assert_contains("Images")
    resp.assert_contains("image=2 slot=1")
    resp.assert_contains("version: 1.104.0")
    hash = resp.assert_get_regx("hash: ([a-f0-9]*)")
    MGRCLI.command("image test %s" % hash)

    SE.send_isp_reset()
    HE.expect("Updating SE image")
    HE.expect("Example app running!", timeout=30)

    SE.send_isp_reset()
    HE.expect("Current SE version: 1.104.0")
    HE.expect("Example app running!")

    SE.send_isp_reset()
    HE.expect("Current SE version: 1.104.0")
    HE.expect("Example app running!")
