#!/usr/bin/env python3.8
""" main of web gui """

import asyncio

import uvicorn

from gui.gui import cam_close


async def main() -> None:
    """ uvicron main """
    config = uvicorn.Config("gui.gui:app", host="0.0.0.0", log_level="debug")
    server = uvicorn.Server(config)
    await server.serve()


if __name__ == "__main__":
    loop = asyncio.get_event_loop()
    try:
        loop.run_until_complete(main())
    except KeyboardInterrupt:
        cam_close()
    finally:
        cam_close()
