# Mace Engine

Proyecto de Unreal Engine 5.8 que sirve de **banco de pruebas** para los plugins de
combate de Mace Engine.

El producto son los plugins de `Plugins/`. Este proyecto existe para desarrollarlos y
probarlos contra contenido real: es el anfitrión, no el entregable.

---

## Requisitos

| | |
|---|---|
| Motor | Unreal Engine **5.8.1** |
| IDE | Visual Studio 2022 con *Desarrollo de juegos con C++* |
| Contenido | **Game Animation Sample Project (GASP) 5.8**, desde el Epic Games Launcher |
| Control de versiones | Git con **Git LFS** instalado (`git lfs install`) |

---

## Puesta en marcha desde un clon limpio

El repositorio ya trae la configuración completa: los 21 plugins activados en
`MaceEngine.uproject`, y en `Config/` los canales de colisión, las Data-Driven CVars,
los Gameplay Tags y los ajustes de skinning y física que GASP necesita.

Lo único que falta tras clonar es el contenido, que **no se versiona** (ver más abajo).

### 1. Instalar GASP

Descarga *Game Animation Sample Project* para 5.8 desde el Epic Games Launcher.

### 2. Migrar el contenido

Abre GASP en el editor y, desde el Content Browser, selecciona estas carpetas:

```
Content/Blueprints
Content/Characters/UEFN_Mannequin
Content/Characters/UE5_Mannequins
Content/Input
Content/Audio
Content/Widgets
Content/Levels/LevelPrototyping
```

Clic derecho → **Migrate…** → destino:

```
<ruta-del-clon>/MaceEngine/Content
```

> El destino debe terminar en `\Content`. Si seleccionas la raíz del proyecto acabarás
> con `Content\Content\…` y hay que borrar y repetir.

`LevelPrototyping` no es opcional: define `LevelBlock_Traversable`, sin la cual
`AC_TraversalLogic` no compila, y es la única geometría que trae el perfil de colisión
`TraversalObjectPreset` asignado.

**El editor de MaceEngine debe estar cerrado durante la migración.**

### 3. Compilar

```powershell
# Clic derecho en MaceEngine.uproject -> Generate Visual Studio project files
# Después, compilar el target MaceEngineEditor | Development Editor | Win64
```

### 4. Verificar

Abre el proyecto y comprueba, en `Content/Sandbox/Levels` o en un nivel propio:

- [ ] El personaje se mueve sin patinar y cambia de marcha con fluidez
- [ ] Los pasos suenan al caminar
- [ ] Saltar sobre un `LevelBlock_Traversable` dispara la animación de traversal
- [ ] El ragdoll se activa y el personaje se levanta
- [ ] `Message Log → Load Errors` está vacío tras reiniciar

Si la consola no reconoce `DDCvar.Traversal.DrawDebugLevel`, la configuración no se
aplicó y el problema está en `Config/`, no en la migración.

---

## Qué se versiona y qué no

| Ruta | Versionado | Motivo |
|---|---|---|
| `Config/` | Sí | La receta completa de la importación |
| `Source/` | Sí | Módulo del proyecto |
| `Plugins/` | Sí | **El producto** |
| `Content/Sandbox/` | Sí | Contenido propio de pruebas |
| `Content/*` (resto) | **No** | Contenido de Epic, reproducible con los pasos de arriba |

GASP son ~3,2 GB de contenido de Epic sin modificar. Versionarlo no aporta nada —nunca
vas a hacer un diff de una animación— y agota la cuota de Git LFS. El repositorio
guarda la receta, no el resultado.

La exclusión está en `.gitignore` como `/Content/*` con `!/Content/Sandbox/`, así que
cualquier carpeta que migres en el futuro queda ignorada automáticamente.

---

## Regla: no modifiques assets de GASP

Si necesitas una versión alterada de algo de GASP —por ejemplo un `SandboxCharacter`
con un componente de combate— **duplícalo a `Content/Sandbox/` y modifica la copia**.

Un cambio hecho sobre un asset de GASP es invisible para git y se pierde en el
siguiente clon. Es la misma disciplina que rige los plugins, extendida a `/Game/`.

---

## Arquitectura

El sistema de combate se reparte en tres plugins con dependencias en un solo sentido:

```
MaceCombat            núcleo. Solo código y datos no visuales.
   ↑                  Sin dependencias experimentales.
MaceCombatAnimation   puente con PoseSearch, Chooser y Motion Warping.
   ↑                  Capa sacrificable ante cambios de versión del motor.
MaceModernCombat      Game Feature: personaje, habilidades y nivel de ejemplo.
```

Dos reglas que no pueden romperse:

1. **Ningún plugin referencia `/Game/`.** Si un asset de un plugin apunta a contenido
   del proyecto, el plugin deja de ser portable y no te enteras hasta que lo pruebes
   fuera de aquí.
2. **Las dependencias solo apuntan hacia abajo.** Nunca al revés, nunca al proyecto.

La prueba de que ambas se cumplen: copiar `Plugins/` a un proyecto en blanco y que
compile y funcione. Conviene repetirla al cerrar cada bloque de trabajo.

---

## Estructura

```
MaceEngine/
├── Config/              Canales de colisión, CVars, tags. Versionado.
├── Content/             GASP (ignorado) + Sandbox/ (versionado)
├── Plugins/             Los plugins de combate. El producto.
├── Source/MaceEngine/   Módulo del proyecto. Mínimo a propósito.
└── MaceEngine.uproject  21 plugins de motor activados
```

---

## Licencia y contenido de terceros

El *Game Animation Sample Project* es propiedad de Epic Games y se distribuye bajo la
licencia de contenido de Unreal Engine. Su uso dentro de un proyecto de UE está
permitido; **redistribuirlo dentro de un plugin no lo está.**

Por eso GASP vive en `Content/` y nunca dentro de `Plugins/`: los plugins deben poder
distribuirse sin él.
