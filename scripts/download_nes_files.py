import re
import pathlib
import html

import aiohttp
import asyncio


def write_to_file(save_directory: str, data: bytes):
    with open(save_directory, "wb") as file:
        file.write(data)


async def download_nes_file(href: str, directory: str):
    directory = pathlib.Path(directory)
    if not (directory.exists() and directory.is_dir()):
        print(f"Directory {directory} is not exists")
        return

    page = "https://www.nesfiles.com" + href

    async with aiohttp.ClientSession() as session:
        async with session.get(page) as response:
            text = await response.text()

            begin = text.find(r'<div class="lal">')
            end = text.find(r'</div>', begin)

            text = text[begin:end]
            li = re.findall(r'<a href="(.*?)">.*?</a>', text)

            if len(li) != 1:
                return

            download_url = "https://www.nesfiles.com" + li[0]

        async with session.get(download_url) as response:
            data = await response.read()
            file_name = html.unescape(
                download_url[download_url.rfind("/") + 1:])

            save_directory = directory.joinpath(file_name)

            if save_directory.exists():
                print(f"File {save_directory} is exists")
            else:
                write_to_file(str(save_directory), data)
                print(f"-> Download {file_name} success")


async def main():
    async with aiohttp.ClientSession() as session:
        async with session.get("https://www.nesfiles.com/Games") as response:
            text = await response.text()

            table_begin = text.find(r'<table class="nesfilesTable">')
            table_end = text.rfind(r'</table>')

            table = text[table_begin:table_end]

            li = re.findall(r'<a href="(.*?)">(.*?)</a>', table)

            tasks = []
            for (href, name) in li:
                print(f"Find {name}, try to download")
                task = download_nes_file(href, "~/Downloads/nes_roms/")
                tasks.append(task)

            await asyncio.gather(*tasks)


if __name__ == '__main__':
    asyncio.run(main())
