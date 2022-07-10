# Import module
- The module name of the plug-in API is bot, which can be imported according to the following code
```python
import bot #Import BOT main module
```

# Initialize logger
- The module provides an API for outputting logs to the BOT console. Initialization can be as follows
```python
import bot
logger = bot.Logger("I'm a test plug-in") #Initialize logger
logger.debug("I am a debug prompt") #Output debug information
logger.info("I am an info prompt") #Output info information
logger.warn("I'm a warning") #Output warn information
logger.error("I am an error prompt") #Output error information
```

# Register plug-ins with cspbot
- Cspbot will call the register function of the plug-in to obtain the registration information so that the information can be displayed in the UI
```python
import bot
def Register():
    return {
        "name": "Plugin name",
        "info": "Plugin introduction",
        "Author":"author",
        "Version":"version number"
    }
```