import pywikibot
import tempfile
import os
from PIL import Image, ImageOps
import subprocess
import time
import urllib

TEXTURE_TILE_SIZE = 128
TEXTURE_CATEGORY = "Category:Quality_images_of_texture_photography"
DELAY_BETWEEN_FILES = 1.0
UNIDENTIFIED_DELAY_BETWEEN_FILES = 5.0

def get_email():
    email = os.environ.get("WIKI_EMAIL", None)
    if email is None:
        try:
            email = subprocess.check_output(["git", "config", "user.email"], text=True, stderr=subprocess.DEVNULL).strip()
            print("Got email from git config:", email)
        except subprocess.CalledProcessError:
            email = None
    return email

email = get_email()
delay_between_files = DELAY_BETWEEN_FILES if email else UNIDENTIFIED_DELAY_BETWEEN_FILES

pywikibot.config.minthrottle = 2
pywikibot.config.maxthrottle = 5

if email:
    pywikibot.config.user_agent = f"TransfurQuiltBot/1.0 (https://github.com/simon0302010/transfur/; {email}) pywikibot/11.9.0"
else:
    print("Warning: No email found for user agent. Set WIKI_EMAIL environment variable or configure git user.email.")
    print("Proceeding without email. Downloads may take longer or be throttled.")

def download_thumbnail(file_page, target_path):
    headers = {"User-Agent": pywikibot.config.user_agent}
    try:
        thumbnail_url = file_page.get_file_url(url_width=TEXTURE_TILE_SIZE * 2)
        req = urllib.request.Request(thumbnail_url, headers=headers)
        with urllib.request.urlopen(req) as response, open(target_path, 'wb') as out_file:
            out_file.write(response.read())
    except Exception as e:
        print(f"Error downloading thumbnail for {file_page.title()}: {e}")


def get_textures():
    with tempfile.TemporaryDirectory() as temp_dir:
        site = pywikibot.Site('commons', 'commons')

        category = pywikibot.Category(site, TEXTURE_CATEGORY)

        for page in category.members(member_type='file'):
            try:
                file_page = pywikibot.FilePage(page)

                file_name = file_page.title(with_ns=False)

                file_path = os.path.join(temp_dir, file_name)

                print(f"Downloading {file_name}...")

                download_thumbnail(file_page, file_path)

                #time.sleep(delay_between_files)
            except Exception as e:
                print(f"Error downloading: {e}")

        files = [f for f in os.listdir(temp_dir) if os.path.isfile(os.path.join(temp_dir, f))]

        total_width = TEXTURE_TILE_SIZE * len(files)
        total_height = TEXTURE_TILE_SIZE

        strip = Image.new("RGB", (total_width, total_height), (0,0,0,0))

        for i, file_name in enumerate(files):
            path = os.path.join(temp_dir, file_name)
            with Image.open(path) as img:
                tile = ImageOps.fit(
                    img,
                    (TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE),
                    method=Image.Resampling.LANCZOS,
                    centering=(0.5, 0.5)
                )

                x_offset = i * TEXTURE_TILE_SIZE

                strip.paste(tile, (x_offset, 0))
    return strip



def main() -> None:
    print("Getting textures...")
    textures_strip = get_textures()
    textures_strip.save("../assets/textures_strip.png")
    print("Saved textures_strip.png to assets")

if __name__ == "__main__":
    main()