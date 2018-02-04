
#include <gtk/gtk.h>

#define	WIDTH	600
#define	HEIGHT	400

#define	NELEM(X) (sizeof(X)/sizeof(X[0]))

GtkWidget *drawing_area;
int first = 1;
double cq[60];
double mincq,maxcq,mean;
double mq[60];


/* Another callback */
static void destroy( GtkWidget *widget,  gpointer   data )
{
    gtk_main_quit ();
}


void draw_line(cairo_t *cr,int x,int y,int x2,int y2)
{
    cairo_move_to(cr,x,y);
    cairo_line_to(cr,x2,y2);
}

int scale(double y)
{
    return HEIGHT-10 - (int)((HEIGHT-20.)*(y-mincq)/(maxcq-mincq));
}


/* Redraw the screen from the backing pixmap */
static gboolean
expose_event( GtkWidget *widget, GdkEventExpose *event )
{
int i;
int sx = WIDTH / (NELEM(cq)-1);
int ox = (WIDTH - sx * (NELEM(cq)-1)) / 2;

    cairo_t *cr = gdk_cairo_create (gtk_widget_get_window (widget));
    cairo_set_line_width(cr, 0.5);

    cairo_set_source_rgba(cr, 0, 0, 1, 1);
    for(i=0; i < NELEM(cq)-1; i++) {
        draw_line(cr,ox+sx*i,scale(cq[i]),ox+sx*(i+1),scale(cq[i+1]));
    }
    cairo_stroke(cr);

    cairo_set_line_width(cr, 0.5);
    cairo_set_source_rgba(cr, 1, 0, 0, 1);
//    draw_line(cr,ox,scale(mean),WIDTH-ox,scale(mean));
    for(i=0; i < NELEM(cq)-1; i++) {
        draw_line(cr,ox+sx*i,scale(mq[i]),ox+sx*(i+1),scale(mq[i+1]));
    }
    cairo_stroke(cr);

    cairo_destroy (cr);
    return FALSE;
}


//==============================================================================

void calc_scale(double csq)
{
int i;
    if(first) {
	for(i=0; i < NELEM(cq); i++) {
	    cq[i] = csq;
	    mq[i] = csq;
	}
	first = 0;
    }

    for(i=1; i < NELEM(cq); i++) {
	cq[i-1] = cq[i];
	mq[i-1] = mq[i];
    }
    cq[NELEM(cq)-1] = csq;

    mean = mincq = maxcq = cq[0];
    for(i=1; i < NELEM(cq); i++) {
	if(cq[i] > maxcq) maxcq = cq[i];
	if(cq[i] < mincq) mincq = cq[i];
	mean += cq[i];
    }
    mean /= NELEM(cq);
    mq[NELEM(cq)-1] = mean;

    double d = 100.*(maxcq-mincq)/maxcq;
    printf("mincq = %5.2f maxcq = %5.2f mean = %5.2f d = %5.2f\n",mincq,maxcq,mean,d);
}


void draw(double csq)
{
    calc_scale(csq);
    gtk_widget_queue_draw_area(drawing_area,0,0,WIDTH,HEIGHT);
}

//==============================================================================


void loop(void (*cb)(double));	//from alsa.c


void draw1(double csq)
{
    gdk_threads_enter();
    draw(csq);
    gdk_flush();
    gdk_threads_leave();
}


void *draw_thread(void *args)
{
    loop(draw1);
    return NULL;
}



int main( int   argc,   char *argv[] )
{
    /* GtkWidget is the storage type for widgets */
    GtkWidget *window;

    /* init threads */
    gdk_threads_init();
    
    /* This is called in all GTK applications. Arguments are parsed
     * from the command line and are returned to the application. */
    gtk_init (&argc, &argv);
    
    /* create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    /* Here we connect the "destroy" event to a signal handler.  
     * This event occurs when we call gtk_widget_destroy() on the window,
     * or if we return FALSE in the "delete-event" callback. */
    g_signal_connect (window, "destroy",  G_CALLBACK (destroy), NULL);
    
    /* Sets the border width of the window. */
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request (drawing_area, WIDTH, HEIGHT);

    gtk_signal_connect (GTK_OBJECT (drawing_area), "expose_event",
	      (GtkSignalFunc) expose_event, NULL);

    gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK);

    gtk_container_add (GTK_CONTAINER (window), drawing_area);
    
    /* The final step is to display this newly created widget. */
    gtk_widget_show (drawing_area);
    
    /* and the window */
    gtk_widget_show (window);
    
    if (!g_thread_new("draw",draw_thread, NULL)) {
      g_printerr ("Failed to create thread");
      return 1;
    }

    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or
     * mouse event). */
    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();
    return 0;
}

