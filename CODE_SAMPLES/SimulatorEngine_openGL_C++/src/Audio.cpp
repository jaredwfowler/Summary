//Dependencies======================================================================================
//==================================================================================================
#include "Audio.hpp"

//Method Definitions================================================================================
//==================================================================================================
Audio::Audio(const char* _id, const char* _filePath)
   :Element(_id)
{
   this->engine = irrklang::createIrrKlangDevice();
   if (this->engine == NULL)
   {
      fprintf(stderr, "Failed to initiate Audio Engine\n");
      exit(AUDIO_FAIL);
   }
   if (_filePath == NULL)
   {
      fprintf(stderr, "FILE cannot be NULL\n");
      exit(AUDIO_FAIL);
   }
   strncpy(this->file.file, _filePath, MAX_FILE_LEN - 1);
}

//**************************************************************************************************

Audio::~Audio(void)
{
   this->engine->drop();
}

//**************************************************************************************************

FILE_GL Audio::getFile(void)
{
   return this->file;
}

//**************************************************************************************************

void Audio::play(void)
{
   if (!this->engine->isCurrentlyPlaying(this->file.file))
   {
      this->engine->play2D(this->file.file);
   }
}

//**************************************************************************************************

void Audio::stop(void)
{
   if (this->engine->isCurrentlyPlaying(this->getFile().file))
   {
      this->engine->stopAllSounds();
   }
}

//**************************************************************************************************

void Audio::setVolume(float v)
{
   if (v < 0)
   {
      v = 0;
   }
   else if (v > 1)
   {
      v = 1;
   }

   this->engine->setSoundVolume(v);
}
