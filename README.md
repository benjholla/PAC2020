# PAC2020-2026
Program Analysis for Cybersecurity (PAC) Course Materials

## Lab Virtual Machines
The username and password of both the HackingLive and PAC2026 VM is `pac`:`badpass`.

Malware samples are password protected as zip files with the password: `infected`.

Both virtual machines are playable in VirtualBox versions 7.0, which is freely
available for Windows, Linux, and Mac at: https://www.virtualbox.org/wiki/Downloads.

The HackingLive.ova is known to NOT work with VMWare products, however the PAC2026.ova
MAY work with VMWare (although it has not been officially tested). It is recommended
to use VirtualBox to import these VMs.

```
HackingLive.ova (https://drive.google.com/file/d/10jA-ASbqNxjGaYdC5JFJFcVtL7OvWyTd/view)
MD5: B9AA4276BD3F0D139139D48EE1F01D16
SHA-1: 49FFB835F27958DA89BC1EECDA317980FFA6597A

PAC2026.ova (https://drive.google.com/file/d/1H8SsFk3y91PxOHW5k6TJ9ml575OCBKH0/view)
MD5: 08548d62f0a61ac1ed618eb2d5b3320e
SHA-1: 57913b27e33db98f256e34c7f174724d6c91b656

en_windows_xp_professional_with_service_pack_3_x86.iso (https://drive.google.com/file/d/1S2cg51Zm5ZT17ERQ5avpWotJToVEpMED/view)
MD5: F424A52153E6E5ED4C0D44235CF545D5
SHA-1: 1C735B38931BF57FB14EBD9A9BA253CEB443D459
```

License
-------------------------------------------------------------------------------

Except where other licenses apply these materials are released under The MIT License.
See the PAC2026.pdf file for more details.

## Notes on Building the VM

Slack space was zero'd using command `sudo dd if=/dev/zero of=bigfile bs=8M status=progress; sudo rm bigfile` and then shutdown and then freed using VirtualBox compact utility `"C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" modifymedium --compact "C:\Users\developer\VirtualBox VMs\PAC 2026\PAC2026-disk001.vdi"` and then finally exported as an OVA with OVF 1.0 specification.
