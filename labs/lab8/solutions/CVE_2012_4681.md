CVE-2012-4681-Armoring
======================

## Overview

A manual antivirus evasion armoring experiment for [CVE-2012-4681](https://cve.mitre.org/cgi-bin/cvename.cgi?name=2012-4681) inspired by [http://security-obscurity.blogspot.com/2012/11/java-exploit-code-obfuscation-and.html](http://security-obscurity.blogspot.com/2012/11/java-exploit-code-obfuscation-and.html).  

Base Exploit: [http://pastie.org/4594319](http://pastie.org/4594319)

## Results as of 9/26/2014 and 8/2/2016

| **Sample**             | **Notes**                                  | **2014 Score (positive detections)**                                                                                              | **2016 Score (postive detections)**                                                                                               |
|------------------------|--------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------|
| **Original Sample**    | http://pastie.org/4594319                  | [30/55](https://www.virustotal.com/en/file/d21171473400807bf969e037aca6105a8b73bed804100fd696d1f46c12853d57/analysis/1411707102/) | [36/56](https://www.virustotal.com/en/file/d21171473400807bf969e037aca6105a8b73bed804100fd696d1f46c12853d57/analysis/1470183524/) |
| **Technique A**        | Changed Class/Method names                 | [28/55](https://www.virustotal.com/en/file/efa71053583c000076bb5a80aea3aebd2b755c8fec29991683bf35a86f7eac44/analysis/1411706956/) | [36/56](https://www.virustotal.com/en/file/efa71053583c000076bb5a80aea3aebd2b755c8fec29991683bf35a86f7eac44/analysis/1470185349/) |
| **Techniques A and B** | Obfuscate strings                          | [16/55](https://www.virustotal.com/en/file/e7243e29592793a604f0945e1f47c49c77416b49b2764a5b334e426f0951ad4e/analysis/1411705332/) | [22/56](https://www.virustotal.com/en/file/e7243e29592793a604f0945e1f47c49c77416b49b2764a5b334e426f0951ad4e/analysis/1470185449/) |
| **Techniques A-C**     | Change Control Flow                        | [16/55](https://www.virustotal.com/en/file/c5dadae9aed4725128b831535b1909b6b2490f8c9d9413f13b34d3918e31b5b5/analysis/1411705224/) | [22/56](https://www.virustotal.com/en/file/c5dadae9aed4725128b831535b1909b6b2490f8c9d9413f13b34d3918e31b5b5/analysis/1470185533/) |
| **Techniques A-D**     | Reflective invocations (on sensitive APIs) | [3/55](https://www.virustotal.com/en/file/82606c62c7e8668d73c28a8c9a2a5d11d06d9cd2c22c7222dc7203d8ee223a9f/analysis/1411706854/)  | [16/56](https://www.virustotal.com/en/file/1e4de34658077beb3d02c1f87ebf04e6ce13eb54f152d019184584428610f907/analysis/1470185597/) |
| **Techniques A-E**     | Simple XOR Packer                          | [0/55](https://www.virustotal.com/en/file/1681522b34b96eff62aad072526baf9f2a9aca22c6988cca0f92d6eec6006a81/analysis/1411708071/)  | [0/56](https://www.virustotal.com/en/file/87fbc91d2a7d72c2f3360614a8d8a790c9a0de3806ede3ff7b1c10ab7cb46fca/analysis/1470185702/)  |
