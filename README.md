# Hellcard - Fast, Pretty, Powerful Flashcard Colorscheme&Wallpaper Generator

<p align="center">
    <img src="https://github.com/user-attachments/assets/bb7ff25c-1d2b-4ac5-a863-c9b32725e965" alt="Img" width="250">
    <img src="https://github.com/user-attachments/assets/3c1b52d2-0336-440b-86d0-85eeba2c8870" alt="Img" width="300">
    <img src="https://github.com/user-attachments/assets/227a7d70-cb5f-4a8d-b89e-bed9d8418a45" alt="Img" width="250">
</p>

<details>
  <summary>Showcase Gallery</summary>
    <p align="center">
      <img src="https://github.com/user-attachments/assets/03dd67af-613d-4d76-900e-bfa76ab16ae2" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/6a822429-2b14-4e72-bdd5-abbe48aabac3" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/248f708e-74c6-4b51-951d-2a97876e0ffe" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/6318d36f-054f-40a5-ba4d-1ad16e9e0ee3" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/65c9c1e4-b080-4de4-ba42-dda8ac3bd66e" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/d1715bad-431a-41d5-88f2-35afc751d4ff" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/27eead7f-1a53-44f9-bc77-ae346d0cef7e" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/454b7af0-9d42-42ad-9be5-888763d0c27a" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/9ca43596-28a2-4980-8975-b3ba43c7295d" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/15d223de-f51a-47a8-adba-a42f4a2fad2c" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/f838ac57-7481-4d0d-8c53-b6bc92debdd6" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/e28c0039-bd42-4d6b-8572-ba43c9e87e7c" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/be187b90-3668-4670-9550-084e043d5d3f" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/d62d6bf4-f372-4370-848c-ecb6081d9bf5" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/ae9a6445-20a9-4a62-b2d6-dc4ce16ae990" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/67820a3d-6f86-4778-b10d-5f56f8d38288" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/b0bbb664-df97-467a-9711-ae144f73472e" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/92e9a4ac-b821-41ee-bd39-b935a7bf770c" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/0507087c-3758-4a9e-9c81-e96a35102690" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/361a3d27-c057-4ead-b834-25e505303c23" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/39befe84-91e2-40e1-bc8a-8b7000865da9" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/bc2aadcb-0b2b-44b5-9b1a-7190b559866d" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/5e046724-beaf-44b6-9311-04fb96928a86" alt="Img" width="250">
      <img src="https://github.com/user-attachments/assets/11e26d33-09d6-45e0-83bf-6ed2f7999356" alt="Img" width="250">
    </p>
</details>

### Star History

<details>
  <summary>Click to expand star history</summary>
    <p align="right">
      <img
        alt="Star History Chart"
        src="https://api.star-history.com/svg?repos=danihek/hellcard&type=Date&theme=dark"
      >
    </p>
</details>

## Building

Clone git repo, run make command and you are ready to go! - you just need C compiler and gnumake!

```sh
git clone https://github.com/danihek/hellcard && cd hellcard && make
```

## Usage

> [!WARNING]
> For now, because of early development hellcard only supports **hellwal** as a **backend**, but other color palettes generators will be supported as well in the future.

Copy [colors.hellwal](https://github.com/danihek/hellwal/blob/main/templates/colors.hellwal) template from hellwal [repo](https://github.com/danihek/hellwal), put it in ``~/.config/hellwal/templates`` folder and run hellwal with your wallpaper.

```sh
hellwal -i wallpaper.png
```

> this will generate **colors.hellwal** in your cache folder in ``~/.cache/hellwal/`` folder.
> if you dont want to color your terminals, use ``--skip-term-colors``

Now you can run hellcard with same wallpaper, preffered style (1-3) and some width:

```sh
./hellcard -i wallpaper.png -s 2 -w 2048
```

...And thats it! You can try other styles or even create your own by modifying the source code.

## Optimizing it for large subset of wallpaper

like here: [reddit](https://www.reddit.com/r/unixporn/comments/1k5mcww/oc_i_created_a_tool_to_create_flashcards/)

You can create a script that is in synergy with hellwal, so you can use $wallpaper variable, and easly fetch any values. It will be even more useful in the future.

Example script that will show hellcard after changing wallpaper:

```sh
#!/usr/bin/env sh

# Generate colorscheme from random wallpaper from folder
hellwal -i $HOME/wallpapers/ -r

# Source all variables like $wallpaper, $color0-15
source ~/.cache/hellwal/variables.sh

# Run hellcard to generate hellcard :p
./hellcard -i $wallpaper -s 2 -o "$HOME/.cache/hellcard.png" -w 1024

# Splash this work of art at your screen with image viewer!
imv "$HOME/.cache/hellcard.png"

```

Example script for all your wallpapers:

```sh
#!/usr/bin/env sh

for w in $(ls -1 ~/wallpapers); do
    hellwal -i $HOME/wallpapers/$w
    source ~/.cache/hellwal/variables.sh

    ./hellcard -i $wallpaper -s 1 -o "$(echo $w)_out1.png" -w 4096
    ./hellcard -i $wallpaper -s 2 -o "$(echo $w)_out2.png" -w 4096
    ./hellcard -i $wallpaper -s 3 -o "$(echo $w)_out3.png" -w 4096
done
```
