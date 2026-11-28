# Playing RSDKv4+ on iPhone or iPad (no Mac needed)

This guide shows how to put the RSDKv4+ app on your iPhone or iPad and play **your own copy** of Sonic 1 or Sonic 2 (2013 mobile version) with it.

> **No game files come with this app.** You need the `Data.rsdk` file from a copy of the game that you own. This project doesn't give out game files, and you shouldn't download them from websites.

---

## 1. What you need

- **An iPhone or iPad** (iOS 13 or newer) and its USB cable.
- **A Windows PC or a Mac.** You only need it for installing (and for refreshing the app every 7 days, see step 8).
- **A free Apple ID.** The one you already use for the App Store is fine. Some people make a separate Apple ID just for sideloading.
- **Your own `Data.rsdk`** from the **2013 mobile** version of **Sonic 1** or **Sonic 2**.
  - ❌ **Sonic Origins data does NOT work.** Origins uses a newer engine format (RSDKv5U). This app is RSDKv4, so it can't read those files.
  - The [RSDKv4 decompilation README](https://github.com/RSDKModding/RSDKv4-Decompilation) explains where the `Data.rsdk` comes from.

## 2. Download the app file (`RSDKv4.ipa`)

1. On your computer, open the **Releases** page: <https://github.com/Yello4/rsdkv4-plusios/releases>
2. Find the newest release that starts with **`ios-v`**.
3. Under **Assets**, click **`RSDKv4.ipa`** to download it.

The `.ipa` is "unsigned". That's normal: Sideloadly or AltStore signs it with your Apple ID while it installs.

## 3. Install it with Sideloadly (easiest on Windows)

1. **Windows only:** install **iTunes** and **iCloud** from Apple's website (the *non*-Microsoft Store versions). Sideloadly's website links to the right ones. On a Mac you don't need this.
2. Download and install **Sideloadly** from its official website: <https://sideloadly.io>
3. Plug your iPhone into the computer with the cable. If your iPhone asks **"Trust This Computer?"**, tap **Trust** and enter your passcode.
4. Open Sideloadly. Your iPhone should show up in the device box.
5. Drag **`RSDKv4.ipa`** into the Sideloadly window (or click the IPA icon and pick the file).
6. Type your **Apple ID** email into the Apple account box.
7. Click **Start**.
8. When it asks, type your Apple ID **password**. If you get a **two-factor code** on your phone, type that in too.
9. Wait until Sideloadly says **Done**. The **RSDKv4+** app is now on your Home Screen, but it won't open yet. Do step 5 first.

### Or: AltStore / SideStore (short version)

- **AltStore:** install **AltServer** on your PC or Mac from <https://altstore.io>, use it to put AltStore on your iPhone, then in AltStore go to **My Apps**, tap **+** and choose `RSDKv4.ipa`.
- **SideStore** (<https://sidestore.io>) is like AltStore but can refresh apps without a computer after the first setup. Follow their setup guide, then add `RSDKv4.ipa` the same way.

## 4. Let your iPhone open the app

**a) Trust your Apple ID as a developer**

1. Open **Settings > General > VPN & Device Management**.
2. Under **Developer App**, tap your Apple ID email.
3. Tap **Trust "…your email…"**, then **Trust** again.

**b) Turn on Developer Mode (iOS 16 or newer)**

1. Open **Settings > Privacy & Security**.
2. Scroll to the bottom and tap **Developer Mode**. (It shows up after you've sideloaded an app.)
3. Switch it **on** and tap **Restart**.
4. After the iPhone restarts, tap **Turn On** in the popup and enter your passcode.

If you're on iOS 15 or older, skip part b.

## 5. Open the app once (this makes its folder)

Open **RSDKv4+** from your Home Screen. Because there's no game data yet, it shows a message telling you where to put `Data.rsdk`. Tap **OK** and the app closes. That's expected!

Opening it once creates the app's folder (with `settings.ini` and `log.txt` inside), so it can now show up in the Files app.

## 6. Put `Data.rsdk` in the app's folder

The file has to end up in **Files > On My iPhone > RSDKv4+** (on iPad: **On My iPad > RSDKv4+**).

Pick whichever way is easiest:

- **From a Windows PC with iTunes:** plug in the iPhone, open iTunes, click the little phone icon, then **File Sharing**. Click **RSDKv4+** in the list and drag `Data.rsdk` into its documents box. (On a Mac, use **Finder**: click your iPhone in the sidebar, then the **Files** tab.)
- **With iCloud Drive:** upload `Data.rsdk` to iCloud Drive (for example at icloud.com on your PC). On your iPhone, open the **Files** app, find it in **iCloud Drive**, press and hold it, tap **Copy**, go to **On My iPhone > RSDKv4+**, press and hold an empty spot, and tap **Paste**.

**Check the name:** it should be exactly **`Data.rsdk`**, with a capital **D** and nothing extra like `.zip` or ` (1)`. (The app also tries to find `data.rsdk` or `DATA.RSDK`, but `Data.rsdk` is the safe choice.) If you have a `.zip`, tap it in Files to unzip it first.

The file must be **directly in** the RSDKv4+ folder, not inside another folder.

## 7. Play!

Close the app all the way: swipe up from the bottom of the screen (or double-click the Home button), then swipe RSDKv4+ away. Open it again and the game should start.

- **Touch controls** show up on the screen. A Bluetooth game controller (Xbox, PlayStation or MFi) also works.
- Your **saves** and **settings** are kept in the same RSDKv4+ folder.

## 8. The 7-day limit (free Apple ID)

Apps installed with a free Apple ID **stop opening after 7 days**. When that happens (or before it does):

- **Sideloadly:** plug in your iPhone and install `RSDKv4.ipa` again with the **same Apple ID**.
- **AltStore / SideStore:** open the app and tap **Refresh All**.

Reinstalling with the same Apple ID normally keeps your files, but it's smart to copy your saves somewhere safe now and then. A free Apple ID also only lets you have **3 sideloaded apps** at a time.

---

## Troubleshooting

**A message says "No game data found"**
`Data.rsdk` isn't in the right place yet. Go back to step 6 and make sure it's directly inside **On My iPhone > RSDKv4+** with the right name.

**A message says the file is an "RSDKv5 data pack"**
That's Sonic Origins (or Sonic Mania) data, which this app can't use. You need the Data.rsdk from the 2013 mobile game.

**A message says the file "isn't a Retro Engine v4 data pack"**
The file is damaged, didn't finish copying, or is still zipped. Copy it again.

**The app closes right away (crash on launch)**
- Did you trust your Apple ID and turn on Developer Mode (step 4)?
- If it's been 7 days, refresh the app (step 8).
- Look at `log.txt` (see below). If it says `*** CRASH ***`, that line and the lines just above it show where it stopped.

**Black screen**
Wait about 10 seconds, then close the app completely and open it again. If it stays black, check `log.txt`.

**The title screen shows, but the game acts weird or shows a script error**
Your `Data.rsdk` might come from a very early 2013 release. This app is built for the newer mobile versions. Using the data from the newest version of the mobile game usually fixes this.

**`log.txt`: send this if you need help**
Each time the app starts, it writes **`log.txt`** in **Files > On My iPhone > RSDKv4+**. It lists the files the app found, whether `Data.rsdk` loaded, and any errors. Press and hold it, tap **Share**, and attach it when you ask for help (for example in a GitHub issue on this project).

**The RSDKv4+ folder isn't in the Files app**
Open the app once first (step 5), then close and reopen the Files app.

---

*RSDKv4+ is a fan project based on the RSDKv4 decompilation. Sonic the Hedgehog is owned by SEGA. Please only use game files you own.*
