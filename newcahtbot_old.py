import customtkinter as ctk
import requests
import threading
import time
import queue
import os
GEMINI_API_KEY = "AIzaSyBykc__glwO4JWJV5Nab2kO6bsEMM3WF_Q"   
GEMINI_API_URL = "https://generativelanguage.googleapis.com/v1/models/gemini-2.5-flash:generateContent"
PROJECT_CONTEXT = """
You are an AI chatbot that answers questions only related to the Indian Judiciary System.

Your main areas of knowledge:
- Structure and functions of courts in India (Supreme Court, High Court, District Courts)
- Judges, advocates, and their roles
- Legal procedures and management systems in courts
- Case filing, hearings, and judgement processes

Project Overview:
This system helps to manage and organize court-related data such as judges, advocates, case details, and scheduling.
It aims to digitalize court operations and make the judicial process more efficient.

Team Information:
- Project Name: Court Management System
- Group Name: NARSIMHA(#MAP)
- Team Members:
  1. Mokshraj Singh Bhadauriya (Team Leader)
  2. Aditya Gupta
  3. Prashant Singh
- Mentor: Ms. Ayushi
- Institution: Graphic Era Deemed To Be University (Dehradun)
- Department: Computer Science and Engineering

Rules for the chatbot:
1. Always answer questions only about the Court Management System or Indian Judiciary.
2. If a user asks unrelated questions, reply:
   "Sorry, I can only answer questions related to the Court Management System or Indian Judiciary."
3. When asked about the project team or mentor, provide the above details.
4. When explaining technical parts, use simple and clear language.
5. Be concise, professional, and factual.
"""
def ask_gemini(message, retries=3, timeout=15):
    headers = {"Content-Type": "application/json"}
    payload = {
        "contents": [
            {"parts": [{"text": PROJECT_CONTEXT}]},
            {"parts": [{"text": message}]}
        ]
    }

    last_err = None
    for attempt in range(1, retries + 1):
        try:
            response = requests.post(
                f"{GEMINI_API_URL}?key={GEMINI_API_KEY}",
                headers=headers,
                json=payload,
                timeout=timeout
            )
            if response.status_code == 503:
                last_err = f"503 Service overloaded (attempt {attempt})"
                time.sleep(2)
                continue
            if response.status_code != 200:
                return f"Error {response.status_code}: {response.text}"
            data = response.json()
            try:
                return data["candidates"][0]["content"]["parts"][0]["text"]
            except Exception:
                return "Unexpected response structure from Gemini."

        except requests.exceptions.RequestException as e:
            last_err = str(e)
            if attempt < retries:
                time.sleep(2)
                continue
            return f"Connection error: {last_err}"

    return "Service temporarily unavailable. Please try again later."
class JudiciaryChatbotApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Indian Judiciary Chatbot (Gemini AI)")
        self.geometry("900x700")
        ctk.set_appearance_mode("light")
        ctk.set_default_color_theme("blue")
        self._ui_queue = queue.Queue()
        self.main_frame = ctk.CTkFrame(self, corner_radius=16)
        self.main_frame.pack(fill="both", expand=True, padx=16, pady=16)

        title = ctk.CTkLabel(self.main_frame, text="⚖️ Indian Judiciary Chatbot", font=("Arial Rounded MT Bold", 20))
        title.pack(pady=(8, 12))
        self.chat_frame = ctk.CTkScrollableFrame(self.main_frame, width=860, height=480, corner_radius=12)
        self.chat_frame.pack(fill="both", expand=True, padx=8, pady=(4, 10))
        input_frame = ctk.CTkFrame(self.main_frame)
        input_frame.pack(fill="x", pady=(0, 8))

        self.user_entry = ctk.CTkEntry(input_frame, placeholder_text="Ask about the Indian Judiciary System...", width=700)
        self.user_entry.pack(side="left", padx=(12, 8), pady=12, fill="x", expand=True)
        self.user_entry.bind("<Return>", lambda e: self.send_message())

        send_button = ctk.CTkButton(input_frame, text="Send", width=110, command=self.send_message)
        send_button.pack(side="right", padx=(0, 12), pady=12)
        self.log_file = "chat_history.txt"
        if not os.path.exists(self.log_file):
            with open(self.log_file, "w", encoding="utf-8") as f:
                f.write("Chat history\n\n")
        self.add_bot_message("Hello! I am your Indian Judiciary Assistant.\nHow can I help you today?")
        self.after(100, self._process_ui_queue)
    def _log(self, who, text):
        try:
            with open(self.log_file, "a", encoding="utf-8") as f:
                f.write(f"{who}: {text}\n\n")
        except Exception:
            pass
    def _safe_add_bubble(self, sender, message, fg_color):
        try:
            if not self.winfo_exists() or not self.chat_frame.winfo_exists():
                return
            bubble = ctk.CTkFrame(self.chat_frame, fg_color=fg_color, corner_radius=12)
            label = ctk.CTkLabel(bubble, text=message, wraplength=720, justify="left", font=("Arial", 13))
            label.pack(padx=10, pady=6)
            anchor = "e" if sender == "user" else "w"
            bubble.pack(anchor=anchor, pady=5, padx=10)
            try:
               
                canvas = self.chat_frame._canvas  
                self.chat_frame.update_idletasks()
                canvas.yview_moveto(1.0)
            except Exception:
                pass
        except Exception:
            pass

    def add_user_message(self, message):
        self.after(0, lambda: self._safe_add_bubble("user", message, "#c5cae9"))
        self._log("User", message)

    def add_bot_message(self, message):
        self.after(0, lambda: self._safe_add_bubble("bot", message, "#e8eaf6"))
        self._log("Bot", message)
    def send_message(self):
        user_text = self.user_entry.get().strip()
        if not user_text:
            return
        self.user_entry.delete(0, "end")
        self.add_user_message(user_text)
        threading.Thread(target=self._background_get_response, args=(user_text,), daemon=True).start()

    def _background_get_response(self, message):
        typing_tag = f"typing-{time.time()}"
        def show_typing():
        
            if self.winfo_exists() and self.chat_frame.winfo_exists():
                lbl = ctk.CTkLabel(self.chat_frame, text="Typing...", font=("Arial", 12, "italic"))
                lbl.pack(anchor="w", padx=10, pady=5)
                self._last_typing_widget = lbl
        self.after(0, show_typing)

        reply = ask_gemini(message)

        
        def remove_typing_and_add():
            try:
                
                if hasattr(self, "_last_typing_widget"):
                    widget = getattr(self, "_last_typing_widget")
                    try:
                        if widget and widget.winfo_exists():
                            widget.destroy()
                    except Exception:
                        pass
                   
                    try:
                        delattr(self, "_last_typing_widget")
                    except Exception:
                        pass

               
                self._safe_add_bubble("bot", reply, "#e8eaf6")
                self._log("Bot", reply)
            except Exception:
                
                pass

        self.after(0, remove_typing_and_add)

    
    def _process_ui_queue(self):
        while not self._ui_queue.empty():
            item = self._ui_queue.get_nowait()
            
        self.after(100, self._process_ui_queue)


if __name__ == "__main__":
    app = JudiciaryChatbotApp()
    app.protocol("WM_DELETE_WINDOW", app.destroy)
    app.mainloop()
