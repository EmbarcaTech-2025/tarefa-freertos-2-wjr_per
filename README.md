
# Tarefa: Roteiro de FreeRTOS #2 - EmbarcaTech 2025

Autor: **Insira Seu Nome**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, ___ de 2025

---

flowchart LR
  subgraph Core0
    UI[[task_input<br/>prio 3]]
    LOGIC[[task_pet_logic<br/>prio 2]]
    RGB[[task_rgb<br/>prio 0]]
  end
  subgraph Core1
    MATRIX[[task_matrix<br/>prio 1]]
    OLED[[task_oled<br/>prio 1]]
    LIGHT[[task_light<br/>prio 2]]
    FLASH[[task_storage<br/>prio 0]]
  end

  UI --> |feed/play/sleep| LOGIC
  LOGIC --> |event queue| MATRIX & OLED & RGB
  LIGHT --> |dark / light| LOGIC
  LOGIC --> |snapshot| FLASH

---

## 📜 Licença
GNU GPL-3.0.
