#!/usr/bin/env python
# encoding: utf-8

import unittest
import subprocess
import random
from gluster import gfapi


class VolumeInfo:
    def __init__(self, volumeName, bricks, redundancyCount, volumeType,
                 volServer):
        self.volumeName = volumeName
        self.volumeType = volumeType
        self.bricks = bricks
        self.redundancyCount = redundancyCount
        self.volServer = volServer


class SingleMachineDisperseVolume(VolumeInfo):
    def __init__(self, servername, brickCount, redundancyCount):
        name = "disperse-" + str(brickCount) + "-" + str(redundancyCount)
        bricks = [servername + ":/data/bricks" + str(i) + "/data"
                  for i in range(brickCount)]
        VolumeInfo.__init__(self, name, bricks, redundancyCount, "disperse",
                            servername)

    def cleanUp(self):
        for brick in self.bricks:
            subprocess.call('rm -rf ' + brick, shell=True)


class GlusterInstance:
    def __init__(self):
        pass

    def start(self):
        subprocess.call("service glusterd restart", shell=True)

    def createVolume(self, volume):
        # print volume.bricks
        subprocess.check_call(
            ' '.join(["gluster volume create", volume.volumeName,
                      volume.volumeType, str(len(volume.bricks)), "redundancy",
                      str(volume.redundancyCount), "transport", "rdma"] +
                     volume.bricks + ["force"]),
            stderr=subprocess.STDOUT,
            shell=True)

    def startVolume(self, volume):
        subprocess.check_call("gluster volume start " + volume.volumeName+" force",
                              shell=True)

    def stopVolume(self, volume):
        subprocess.check_call(
            "echo 'y' | gluster volume stop " + volume.volumeName + " force",
            shell=True)

    def deleteVolume(self, volume):
        subprocess.check_call(
            "echo 'y' | gluster volume delete " + volume.volumeName,
            shell=True)

    def setMultiThread(self, volume, threads):
        subprocess.check_call("gluster volume set " + volume.volumeName +
                              " disperse.coding-threads " + str(threads),
                              shell=True)
    def disableCache(self,volume):
        subprocess.check_call("gluster volume set " + volume.volumeName+ " performance.io-cache off", shell = True)

    def heal(self,volume):
        subprocess.check_call("gluster volume heal "+ volume.volumeName + " full",shell=True)

    def getGfapi(self, volume):
        return gfapi.Volume(volume.volServer, volume.volumeName)

    def killProcess(self, brick_id):
        subprocess.call('ps -ef | grep bricks' + str(brick_id) +
                        " | awk {'print $2'} | xargs kill -9",
                        shell=True)


def randData(size):
    # BUG: if a byte is 0x00, there will be bug to write it using python.
    return bytearray([random.randint(1, 255) for i in xrange(size)])


class TestGluster(unittest.TestCase):
    def setUp(self):
        self.gluster = GlusterInstance()
        self.volume = SingleMachineDisperseVolume("tfs00", 12, 4)

        self.gluster.start()

        try:
            self.gluster.createVolume(self.volume)
            self.gluster.setMultiThread(self.volume, 8)
            self.gluster.disableCache(self.volume)
            self.gluster.startVolume(self.volume)

        except subprocess.CalledProcessError:

            print "Volume may already exists.\n"
            print "Trying to delete volume.\n"

            self.gluster.stopVolume(self.volume)
            self.gluster.deleteVolume(self.volume)

            print "Successfully Delete Volume.\n"
            print "Trying to create volume again.\n"

            self.gluster.createVolume(self.volume)
            self.gluster.setMultiThread(self.volume, 8)
            self.gluster.disableCache(self.volume)
            self.gluster.startVolume(self.volume)

        #self.gluster.heal(self.volume)
        self.gluster.start()
        self.api = self.gluster.getGfapi(self.volume)
        self.api.mount()

    def tearDown(self):
        pass
        #self.api.umount()
        #self.gluster.stopVolume(self.volume)
        #self.gluster.deleteVolume(self.volume)
        #self.api.umount()

    """
    def testWriteFileWithCrash(self):
        pass
    """

    def testWriteFile(self):
        if not self.api.exists("/testDir"):
            self.api.mkdir('testDir', 0755)

        fd = self.api.fopen("/testDir/new5.txt", 'w+')

        data_size = random.randint(0, 2**20)
        data = randData(data_size)

        #t = Thread(target = unwrap_write,args=(fd,data,))
        to_kill = [random.randint(0, len(self.volume.bricks) - 1)
                   for i in range(self.volume.redundancyCount)]
        print to_kill
        for brick_id in to_kill:
            self.gluster.killProcess(brick_id)
        #t.start()

        #t.join()
        unwrap_write(fd, data)

        fd.close()

        fd = self.api.fopen("/testDir/new5.txt", 'r')
        self.assertEqual(fd.read(-1), data)
        fd.close()


def unwrap_write(fd, data):
    print 'Begin to unwrap write.'
    fd.write(data)
    print 'Finish write.'
    fd.fsync()
    print 'Finish sync.'


if __name__ == '__main__':
    unittest.main()
