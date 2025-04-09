from copy import deepcopy
from os import path
import pexpect
import platform
import pytest
import re
import subprocess


if platform.system() == "Windows":
    _MCUMGR_COMMAND_BASE = ["mcumgr.exe"]
else:
    _MCUMGR_COMMAND_BASE = ["./mcumgr"]


class McumgrResponse:
    def __init__(self, response_string):
        self.response_string = response_string

    # Consumes up to found string
    def assert_contains(self, what):
        pos = self.response_string.find(what)
        assert pos > -1, what + " not found from response: \n" + self.response_string
        self.response_string = self.response_string[pos + len(what):]

    # Doesn't consume
    def assert_doesnt_contain(self, what):
        pos = self.response_string.find(what)
        assert pos == -1, what + " was found from response"

    # Doesn't consume
    def assert_get_regx(self, regex):
        reo = re.compile(regex)
        match = re.search(reo, self.response_string)
        assert match is not None, regex + " not found from response"
        return match.group(1)


class SecureEnclaveDut:
    def __init__(self, se_dut):
        self.se = se_dut

    @staticmethod
    def _se_checksum(cmd_bytes):
        checksum = (((sum(cmd_bytes) + 1) ^ 0xff) + 2) & 0xff
        return cmd_bytes + [checksum]

    def _send_cmd(self, cmd):
        """ Assumes SE is connected. """
        cmd[0] = len(cmd) + 1
        cmd = SecureEnclaveDut._se_checksum(cmd)
        cmd = bytearray(cmd)
        self.se.write(cmd)

    def _start_isp(self):
        cmd_packet = [0x00, 0x00]
        self._send_cmd(cmd_packet)  # this will set SE to ISP mode -> will take commands
        resp = self.se.expect(pexpect.TIMEOUT, timeout=1.0)
        assert b'\3\xfe\xff' in resp  # just assert that ACK is found, there might be other SE output in the buffer

    def _end_isp(self):
        cmd_packet = [0x00, 0x01]
        self._send_cmd(cmd_packet)
        self.se.expect(pexpect.TIMEOUT, timeout=0.1)

    def send_isp_reset(self):
        # start ISP
        self._start_isp()
        # reset device
        cmd_packet = [0x00, 0x9]
        cmd_packet = cmd_packet + list(0x02.to_bytes(4, byteorder='little'))
        self._send_cmd(cmd_packet)
        self.se.expect("\\[SES\\] STOC DEVICE ok", timeout=5.0)


class DutElement:
    def __init__(self, target_dut):
        self.target_dut = target_dut

    def expect(self, what, timeout=5.0):
        self.target_dut.expect(what, timeout=timeout)


class McumgrCLI:
    def __init__(self, exec_path, connection_name, images_path):
        self.exec_path = exec_path
        self.images_path = images_path
        self.cmd_base = deepcopy(_MCUMGR_COMMAND_BASE)
        self.cmd_base.append("-c" + connection_name)

    def command(self, cmd, timeout=5, tries=1):
        spcmd = deepcopy(self.cmd_base)
        spcmd.extend(cmd.split(" "))
        if tries > 1:
            spcmd.append("-r%d" % tries)
        resp = subprocess.check_output(spcmd, cwd=self.exec_path, timeout=timeout)
        return McumgrResponse(resp.decode("UTF-8"))

    def command_image_upload(self, binary_name, image_id=0):
        command = "image upload " + path.join(self.images_path, binary_name)
        if image_id > 0:
            command += " -n%d" % image_id
        return self.command(command, timeout=200)

    def command_and_assert(self, cmd, what, timeout=5, tries=1):
        resp = self.command(cmd, timeout, tries)
        resp.assert_contains(what)


_MGRINSTANCE = None


@pytest.fixture
def SE(dut):
    yield SecureEnclaveDut(dut[0])


@pytest.fixture
def HE(dut):
    yield DutElement(dut[1])


@pytest.fixture
def HP(dut):
    yield DutElement(dut[2])


@pytest.fixture
def MGRCLI(request):
    global _MGRINSTANCE
    if _MGRINSTANCE is None:
        path = request.config.getoption("--mcumgr_path")
        conn = request.config.getoption("--mcumgr_connection")
        images_path = request.config.getoption("--bin_path")
        _MGRINSTANCE = McumgrCLI(path, conn, images_path)
    yield _MGRINSTANCE


@pytest.fixture
def SERAM_UPDATE(request):
    if request.config.getoption("--enable_seram_update") is False:
        pytest.skip("Skipping test requiring SERAM update feature since it's not enabled")
    yield None


def pytest_unconfigure(config):
    # recover cursor to terminal, in case pytest was run with '-s', SE
    # spamming the cursor hiding will cause it to hide from running terminal
    print("\x1b[?25h")


def pytest_addoption(parser):
    parser.addoption(
        "--mcumgr_path",
        action="store",
        help="Path to the mcumgr executable",
    )
    parser.addoption(
        "--mcumgr_connection",
        action="store",
        help="Name of the connection profile for mcumgr.",
        default="uart"
    )
    parser.addoption(
        "--bin_path",
        action="store",
        help="Path to bin folder where FOTA example binaries are",
        default=""
    )
    parser.addoption(
        "--enable_seram_update",
        action="store",
        default=False,
        help="Enable SERAM update testing support, Default is False.",
    )
