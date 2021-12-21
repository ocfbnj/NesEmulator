import asyncio

import aiohttp


async def search(session: aiohttp.ClientSession, ines: int) -> int:
    url = f"https://nesdir.github.io/mapper{ines}.html"

    async with session.get(url) as response:
        html = await response.text()
        count = html.count("<tr>") - 1
        return count


async def main():
    async with aiohttp.ClientSession() as session:
        tasks = []

        for i in range(256):
            task = search(session, i)
            tasks.append(task)

        counts = await asyncio.gather(*tasks)

        for (i, count) in enumerate(counts):
            print(f"ines {i}: {count}")

        total = sum(counts)
        print(f"total: {total}")

        print(f"mapper0-4 cover {(sum(counts[0:5]) / total) * 100:.2f}% games")


if __name__ == '__main__':
    asyncio.run(main())
