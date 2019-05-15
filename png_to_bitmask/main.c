// gimptool-2.0 --install main.c 

// https://developer.gimp.org/writing-a-plug-in/2/index.html
// https://developer.gimp.org/api/2.0/libgimp/index.html
// https://developer.gimp.org/writing-a-plug-in/1/hello.c

// https://www.gimp.org/docs/scheme_plugin/sample_plugin.html

#include <libgimp/gimp.h>

#include <stdio.h>

static void query (void);
static void run   (const gchar      *name,
                   gint              nparams,
                   const GimpParam  *param,
                   gint             *nreturn_vals,
                   GimpParam       **return_vals);

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  NULL,
  query,
  run
};

MAIN()

static void query (void)
{
  static GimpParamDef args[] =
  {
    {
      GIMP_PDB_INT32,
      "run-mode",
      "Run mode"
    },
    {
      GIMP_PDB_IMAGE,
      "image",
      "Input image"
    },
    {
      GIMP_PDB_DRAWABLE,
      "drawable",
      "Input drawable"
    }
  };

  gimp_install_procedure (
    "plug-in-png-to-bitmask",
    "PNG to bitmask",
    "Displays \"PNG to bitmask!\" in a dialog",
    "David Neary",
    "Copyright David Neary",
    "2004",
    "_PNG to bitmask",
    "RGB*, GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS (args), 0,
    args, NULL);

  gimp_plugin_menu_register ("plug-in-png-to-bitmask",
                             "<Image>/Filters/Misc");
}

static void blur (GimpDrawable *drawable)
{
  FILE *border = fopen("/home/cppprosto/tmp/border", "wb");
  FILE *letter = fopen("/home/cppprosto/tmp/letter", "wb");

  GimpPixelRgn rgn_in;
  gint channels;

  gimp_pixel_rgn_init (&rgn_in, drawable, 0, 0, drawable->width, drawable->height, FALSE, FALSE);

  channels = gimp_drawable_bpp (drawable->drawable_id);

  fprintf(border, "// u' '-------------------------------------------------------------------------\n");
  fprintf(border, "static auto rus_??_size {std::make_pair(%i, %i)};\n", drawable->width, drawable->height);
  fprintf(border, "static uint8_t rus_??_border[%i*2] =\n", drawable->height);
  fprintf(border, "{\n");

  fprintf(letter, "static uint8_t rus_??[%i*2] =\n", drawable->height);
  fprintf(letter, "{\n");

  int x = 0;
  int y = 0;
  for(y = 0; y < drawable->height; ++y)
  {
    fprintf(letter, "0b");
    fprintf(border, "0b");

    for(x = 0; x < drawable->width; ++x)
    {
      guchar pixel[4];
      gimp_pixel_rgn_get_pixel (&rgn_in, pixel, x, y);

      if(x > 0)
      {
        if(!(x % 8))
        {
          fprintf(letter, ",0b");
          fprintf(border, ",0b");
        }
      }

      if(pixel[3] == 0)
      {
        fprintf(letter, "0");
        fprintf(border, "0");
      }
      else
      {
        if(pixel[0] == 255 &&
           pixel[1] == 255 &&
           pixel[2] == 255)
        {
          fprintf(letter, "1");
          fprintf(border, "0");
        }
        else
        {
          fprintf(border, "1");
          fprintf(letter, "0");
        }
      }
    }

    for(;x < 16; ++x)
    {
      fprintf(border, "0");
      fprintf(letter, "0");

    }

    if((y+1) < drawable->height)
    {
      fprintf(letter, ",");
      fprintf(border, ",");
    }
    fprintf(letter, "\n");
    fprintf(border, "\n");
  }

  fprintf(letter, "};\n");
  fprintf(border, "};\n");

  fclose(border);
  fclose(letter);
}

static void run (const gchar     *name,
                       gint       nparams,
                 const GimpParam *param,
                       gint      *nreturn_vals,
                       GimpParam **return_vals)
{
  static GimpParam  values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode       run_mode;
  GimpDrawable     *drawable;

  /* Setting mandatory output values */
  *nreturn_vals = 1;
  *return_vals  = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  /* Getting run_mode - we won't display a dialog if
   * we are in NONINTERACTIVE mode */
  run_mode = param[0].data.d_int32;

  /*  Get the specified drawable  */
  drawable = gimp_drawable_get (param[2].data.d_drawable);

  blur(drawable);

  if (run_mode != GIMP_RUN_NONINTERACTIVE)
    g_message("Hello, world TEST!\n");
}


