Wolfenstein 3D
==============

The original source release was made using the ``DEICE.EXE``
installer (DeIce) in the root directory.  (Basically a way to
split self-extracting zip files over multiple floppy disks.)
After execution, that installer will create the contents of
the ``WOLFSRC`` directory.  In the "installed" directory, the
the ``README.TXT``, ``RELEASE.TXT``, and ``LICENCE.DOC`` reside
in the ``README`` directory.  They are duplicated below.

Travis "Ash" Bradshaw <travis.bradshaw@idsoftware.com>

February 6, 2012

RELEASE.TXT
-----------

::

  We are releasing this code for the entertainment of the 
  user community.  We don't guarentee that anything even 
  builds in here.  Projects just seem to rot when you leave 
  them alone for long periods of time.

  This is all the source we have relating to the original 
  PC wolfenstein 3D project.  We haven't looked at this 
  stuff in years, and I would probably be horribly embarassed 
  to dig through my old code, so please don't ask any questions 
  about it. The original project was built in borland c++ 3.0.  
  I think some minor changes were required for later versions.

  You will need the data from a released version of wolf or spear 
  to use the exe built from this code.  You can just use a 
  shareware version if you are really cheap.


  Some coding comments in retrospect:

  The ray casting refresh architecture is still reasonably 
  appropriate for the game.  A BSP based texture mapper could 
  go faster, but ray casting was a lot simpler to do at the time.

  The dynamically compiled scaling routines are now a Bad Thing.  
  On uncached machines (the original target) they are the fastest 
  possible way to scale walls, but on modern processors you just 
  wind up thrashing the code cash and wrecking performance.  
  A simple looping texture mapper would be faster on 486+ machines.

  The whole page manager caching scheme is unecessarily complex.

  Way too many #ifdefs in the code!


  Some project ideas with this code:

  Add new monsters or weapons.

  Add taller walls and vertical motion.   This should only be 
  done if the texture mapper is rewritten.

  Convert to a 32 bit compiler.  This would be a fair amount 
  of work, but I would hate to even mess with crusty old 16 
  bit code.  The code would get a LOT smaller.

  Make a multi-player game that runs on DOOM sersetup / ipxsetup drivers.


  Have fun...

  John Carmack
  Technical Director
  Id Software

README.TXT
----------

NOTES:

This version will compile under BORLAND C++ 3.0/3.1 and compiled perfectly
before it was uploaded. 

Please do not send your questions to id Software.

LICENCE.DOC
-----------

 :: 
  	LIMITED USE SOFTWARE LICENSE AGREEMENT

  	This Limited Use Software License Agreement (the "Agreement") 
  is a legal agreement between you, the end-user, and Id Software, Inc. 
  ("ID").  By continuing the downloading of this Wolfenstein 3D 
  (the "Trademark") software material, which includes source code 
  (the "Source Code"), artwork data, music and software tools 
  (collectively, the "Software"), you are agreeing to be bound by the 
  terms of this Agreement.  If you do not agree to the terms of this 
  Agreement, promptly destroy the Software you may have downloaded.  

  ID SOFTWARE LICENSE

  	Grant of License.  ID grants to you the right to use one (1) 
  copy of the Software on a single computer.  You have no ownership or 
  proprietary rights in or to the Software, or the Trademark.  For purposes 
  of this section, "use" means loading the Software into RAM, as well as 
  installation on a hard disk or other storage device.  The Software, 
  together with any archive copy thereof, shall be destroyed when no longer 
  used in accordance with this Agreement, or when the right to use the 
  Software is terminated.  You agree that the Software will not be shipped, 
  transferred or exported into any country in violation of the U.S. 
  Export Administration Act (or any other law governing such matters) and 
  that you will not utilize, in any other manner, the Software in violation 
  of any applicable law.

  	Permitted Uses.  For educational purposes only, you, the end-user, 
  may use portions of the Source Code, such as particular routines, to 
  develop your own software, but may not duplicate the Source Code, except 
  as noted in paragraph 4.  The limited right referenced in the preceding 
  sentence is hereinafter referred to as "Educational Use."  By so exercising 
  the Educational Use right you shall not obtain any ownership, copyright,
  proprietary or other interest in or to the Source Code, or any portion of
  the Source Code.  You may dispose of your own software in your sole
  discretion.  With the exception of the Educational Use right, you may not
  otherwise use the Software, or an portion of the Software, which includes
  the Source Code, for commercial gain.

  	Prohibited Uses:  Under no circumstances shall you, the end-user, 
  be permitted, allowed or authorized to commercially exploit the Software.
  Neither you nor anyone at your direction shall do any of the following acts
  with regard to the Software, or any portion thereof:

  	Rent;

  	Sell;

  	Lease;

  	Offer on a pay-per-play basis;

  	Distribute for money or any other consideration; or

  	In any other manner and through any medium whatsoever commercially 
  exploit or use for any commercial purpose.

  Notwithstanding the foregoing prohibitions, you may commercially exploit the
  software you develop by exercising the Educational Use right, referenced in
  paragraph 2. hereinabove. 

  	Copyright.  The Software and all copyrights related thereto 
  (including all characters and other images generated by the Software 
  or depicted in the Software) are owned by ID and is protected by 
  United States copyright laws and international treaty provisions.  
  Id shall retain exclusive ownership and copyright in and to the Software 
  and all portions of the Software and you shall have no ownership or other 
  proprietary interest in such materials. You must treat the Software like 
  any other copyrighted material, except that you may either (a) make one 
  copy of the Software solely for back-up or archival purposes, or (b) 
  transfer the Software to a single hard disk provided you keep the original 
  solely for back-up or archival purposes.  You may not otherwise reproduce, 
  copy or disclose to others, in whole or in any part, the Software.  You 
  may not copy the written materials accompanying the Software.  You agree 
  to use your best efforts to see that any user of the Software licensed 
  hereunder complies with this Agreement.

  	NO WARRANTIES.  ID DISCLAIMS ALL WARRANTIES, BOTH EXPRESS IMPLIED,
  INCLUDING BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE WITH RESPECT TO THE SOFTWARE.  THIS LIMITED
  WARRANTY GIVES YOU SPECIFIC LEGAL RIGHTS.  YOU MAY HAVE OTHER RIGHTS WHICH
  VARY FROM JURISDICTION TO JURISDICTION.  ID DOES NOT WARRANT THAT THE
  OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED, ERROR FREE OR MEET YOUR
  SPECIFIC REQUIREMENTS.  THE WARRANTY SET FORTH ABOVE IS IN LIEU OF ALL OTHER
  EXPRESS WARRANTIES WHETHER ORAL OR WRITTEN.  THE AGENTS, EMPLOYEES,
  DISTRIBUTORS, AND DEALERS OF ID ARE NOT AUTHORIZED TO MAKE MODIFICATIONS TO
  THIS WARRANTY, OR ADDITIONAL WARRANTIES ON BEHALF OF ID. 

  	Exclusive Remedies.  The Software is being offered to you free of any
  charge.  You agree that you have no remedy against ID, its affiliates,
  contractors, suppliers, and agents for loss or damage caused by any defect
  or failure in the Software regardless of the form of action, whether in
  contract, tort, includinegligence, strict liability or otherwise, with
  regard to the Software.  This Agreement shall be construed in accordance
  with and governed by the laws of the State of Texas.  Copyright and other
  proprietary matters will be governed by United States laws and international
  treaties.  IN ANY CASE, ID SHALL NOT BE LIABLE FOR LOSS OF DATA, LOSS OF
  PROFITS, LOST SAVINGS, SPECIAL, INCIDENTAL, CONSEQUENTIAL, INDIRECT OR OTHER
  SIMILAR DAMAGES ARISING FROM BREACH OF WARRANTY, BREACH OF CONTRACT,
  NEGLIGENCE, OR OTHER LEGAL THEORY EVEN IF ID OR ITS AGENT HAS BEEN ADVISED
  OF THE POSSIBILITY OF SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY.
  Some jurisdictions do not allow the exclusion or limitation of incidental or
  consequential damages, so the above limitation or exclusion may not apply to
  you.

  	General Provisions.  Neither this Agreement nor any part or portion 
  hereof shall be assigned, sublicensed or otherwise transferred by you.  
  Should any provision of this Agreement be held to be void, invalid, 
  unenforceable or illegal by a court, the validity and enforceability of the 
  other provisions shall not be affected thereby.  If any provision is 
  determined to be unenforceable, you agree to a modification of such 
  provision to provide for enforcement of the provision's intent, to the 
  extent permitted by applicable law.  Failure of a party to enforce any 
  provision of this Agreement shall not constitute or be construed as a 
  waiver of such provision or of the right to enforce such provision.  If 
  you fail to comply with any terms of this Agreement, YOUR LICENSE IS 
  AUTOMATICALLY TERMINATED and you agree to the issuance of an injunction 
  against you in favor of Id.  You agree that Id shall not have to post 
  bond or other security to obtain an injunction against you to prohibit 
  you from violating Id's rights.

  	YOU ACKNOWLEDGE THAT YOU HAVE READ THIS AGREEMENT, THAT YOU 
  UNDERSTAND THIS AGREEMENT, AND UNDERSTAND THAT BY CONTINUING THE 
  DOWNLOADING OF THE SOFTWARE, YOU AGREE TO BE BOUND BY THIS AGREEMENT'S 
  TERMS AND CONDITIONS. YOU FURTHER AGREE THAT, EXCEPT FOR WRITTEN SEPARATE 
  AGREEMENTS BETWEEN ID AND YOU, THIS AGREEMENT IS A COMPLETE AND EXCLUSIVE 
  STATEMENT OF THE RIGHTS AND LIABILITIES OF THE PARTIES.  THIS AGREEMENT 
  SUPERSEDES ALL PRIOR ORAL AGREEMENTS, PROPOSALS OR UNDERSTANDINGS, AND 
  ANY OTHER COMMUNICATIONS BETWEEN ID AND YOU RELATING TO THE SUBJECT MATTER 
  OF THIS AGREEMENT 
  
