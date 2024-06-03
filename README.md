# PAC2020
Program Analysis for Cybersecurity (PAC) Course Materials

## Lab Virtual Machines
The username and password of both the HackingLive and PAC2023 VM is `pac`:`badpass`.

Malware samples are password protected as zip files with the password: `infected`.

Both virtual machines are playable in VirtualBox version 7.0, which is freely available
for Windows, Linux, and Mac at: https://www.virtualbox.org/wiki/Downloads.

The HackingLive.ova is known NOT to work with VMWare products, however the PAC2023.ova
MAY work with VMWare (although it has not been officially tested). It is recommended
to use VirtualBox to import these VMs.

```
HackingLive.ova (https://drive.google.com/file/d/1NKbmchgGbiRg0Rno9j0kZxfdZy0dj4B-/view)
MD5: B9AA4276BD3F0D139139D48EE1F01D16
SHA-1: 49FFB835F27958DA89BC1EECDA317980FFA6597A

PAC2024.ova (https://drive.google.com/file/d/1xjzOdjCZLxxrfTNnw7c3mtpf2k4XeNhF/view)
MD5: B9EB49EBA92EEB473D2107ADBDDB4091
SHA-1: AFA1A8ED0356C1C90B64D987F9622A89FFD09E4A

en_windows_xp_professional_with_service_pack_3_x86.iso (https://drive.google.com/file/d/1Ptmy2_X_HtC9YZ_WRKLlmlol_E3TxzUn/view)
MD5: F424A52153E6E5ED4C0D44235CF545D5
SHA-1: 1C735B38931BF57FB14EBD9A9BA253CEB443D459
```

License
-------------------------------------------------------------------------------

Except where other licenses apply these materials are released under The MIT License.
See the PAC2023.pdf file for more details.

## Notes on Building the VM

Slack space was zero'd using command `sudo dd if=/dev/zero of=bigfile bs=8M status=progress; sudo rm bigfile` and then shutdown and then freed using VirtualBox compact utility `"C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" modifymedium --compact "C:\Users\developer\VirtualBox VMs\PAC 2021\PAC2020-disk001.vdi"` and then finally exported as an OVA with Virtual Box 6.1.
