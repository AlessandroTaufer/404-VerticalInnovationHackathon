import speech_recognition as sr
from gtts import gTTS
from playsound import playsound
import os
import time

from db import Interface


def callback(recognizer, audio):
    print("callback")
    # received audio data, now we'll recognize it using Google Speech Recognition
    try:
        input = recognizer.recognize_google(audio)
        print("Google Speech Recognition thinks you said " + input)
        if "hey assistant" in input or ("find" in input and "tool" in input) or "help" in input:
            playsound("object.mp3")
            try:
                with sr.Microphone() as source:
                    audio = r.listen(source)

                text = r.recognize_google(audio)
                text = text.split(" ")[-1:]
                # print("Google Speech Recognition thinks you said " + text)
            except sr.UnknownValueError:
                print("Google Speech Recognition could not understand audio")
                text = ""
            except sr.RequestError as e:
                print("Could not request results from Google Speech Recognition service; {0}".format(e))
                text = ""
            if text != "":
                obj = i.findobject(text[0])
                # print(obj)
                if obj != -1:
                    ret = obj.fetchall()[0][0]
                    tts = gTTS("Your {} is at: {}".format(text, ret))
                else:
                    tts = gTTS("Sorry i could not find your {}".format(text))
            else:
                tts = gTTS("Sorry i could not understand what you've said")
            tts.save("{}.mp3".format(text))
            playsound("{}.mp3".format(text))
            os.remove("{}.mp3".format(text))

    except sr.UnknownValueError:
        print("Google Speech Recognition could not understand audio")
    except sr.RequestError as e:
        print("Could not request results from Google Speech Recognition service; {0}".format(e))


if __name__ == "__main__":

    i = Interface("tools", "postgres", "ivan127")
    i.connect()
    r = sr.Recognizer()


    m = sr.Microphone()
    with m as source:
        r.adjust_for_ambient_noise(source, 0.5)

    stop_listening = r.listen_in_background(m, callback)

    while True:
        time.sleep(1)