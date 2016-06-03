;;; Copyright (C) 1992  SAS Institute.
;;; 
;;; This program is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 1, or (at your option)
;;; any later version.
;;; 
;;; This program is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;; 
;;; You should have received a copy of the GNU General Public License
;;; along with this program; if not, write to the Free Software
;;; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;;; 

;;;---------------------------------------------------------
;;; This file contains el code for SAS mode and C comment mode.
;;; If this file is too large for your mailer, it can be sent in
;;; two parts. The C comment file starts at about line 905.
;;;
;;;
;;; Electric SAS mode - takes care of indenting and flashing and filling
;;;   for SAS code.
;;;  file name: sas-mode.el
;;;  Author:   Mark Riggle / J4
;;;            AI Dept
;;;            SAS Institute Inc.
;;;            SAS Campus Dr.
;;;            Cary, NC 27513
;;;            sasmsr@unx.sas.com
;;;
;;;   Inspired from the C-mode of GNU and the desperation of people
;;;     used to using a LISP environment needing to write SCL code.
;;;
;;; Everyone is granted permission to copy, modify and redistribute
;;; this file, provided the people they give it to can and that this
;;; header remains with the file.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;   Also see c-fill.el for where the fill mode came from.from.
;;;
;;;  ad the following lines to your .emacs file
;;;  (autoload  'sas-mode   "sas-mode")
;;;  (setq auto-mode-alist 
;;;     (append '(("\\.*sas" . sas-mode) ("\\.*scl" . sas-mode))
;;;             auto-mode-alist))

;;;---  Functions useful here are;
;;;
;;; sas-indent-command - bound to tab
;;; sas-indent-region
;;; sas-flash-open
;;; sas-labels-toggle-standard
;;; sas-big-pic
;;; sas-forward-function
;;; sas-backward-function
;;;
;;;  Bind the above functions to mode specific  keys
;;;   in the sas-mode-load-hook by something like this:
;;;  (but use function keys, change the ESC Control sequence)
;;;
;;;   (defun sas-mode-load-hook()
;;;     "set up SAS mode function keys"
;;;     (interactive)
;;;     (define-key sas-mode-map "\e\C-q" 'sas-indent-region)
;;;     (define-key sas-mode-map "\e\C-d" 'sas-forward-function)
;;;     (define-key sas-mode-map "\e\C-b" 'sas-backward-function)
;;;     (define-key sas-mode-map "\e\C-p" 'sas-big-pic)
;;;     (define-key sas-mode-map "\e\C-l" 'sas-flash-open)
;;;     (define-key sas-mode-map "\e\C-i" 'sas-indent-command)
;;;     (define-key sas-mode-map "\e\C-n" 'c-comment)
;;;     (define-key sas-mode-map "\e\C-c" 'exit-recursive-edit))
;;;
;;; If you are not sure what your function keys send as a string to
;;;  Emacs, see the function keyboard-read-key provided at the end of 
;;;  this file.
;;;
;;;
;;;---   important user variables are
;;; 
;;; sas-labels-standard
;;; sas-tab-always-indent
;;; sas-undent-end
;;; sas-continued-statement-offset
;;; sas-continue-to-indent
;;; sas-initial-indent
;;; sas-indent-level
;;; sas-flash-pause
;;; sas-flash-end
;;; sas-electric-semi
;;; sas-auto-newline
;;; sas-fast-scan


; put underscore as a word char
;;; ----------------- user vars -----------------

(defvar sas-auto-newline t
  "*Non-nil means TAB in SAS mode should always reindent the current line,
    regardless of where in the line point is when the TAB command is used.")
(defvar sas-undent-end nil
  "*Controls where an end statement is indented to. T makes it indent at the
    do or select, nil indents the end as an inside statement.")

(defvar sas-tab-always-indent t "*make tab always indent")
(defvar sas-continued-statement-offset 3
  "*Indent for continued statements")
(defvar sas-initial-indent 3
  "*Indent for top level statements")
(defvar sas-preamble-indent 0
  "*Statements found to be in the preamble are indented this much")
(defvar sas-indent-level 3
  "*Indent after DO's and SELECTs.")
(defvar sas-labels-standard t
 "*Buffer-specific: If true then labels that are left justified are
   considered to be like subroutines, otherwise , use some indent.
   This will speed up the indenting process")
(make-variable-buffer-local 'sas-labels-standard)
(defvar sas-flash-pause 1 "*pause n seconds for flash")
(defvar sas-auto-newline t
  "*Insert new-line and indent after an electric ';'")
(defvar sas-electric-semi t
  "*Treat ';' as an indenter command also ans maybe add a newline,
    see sas-auto-newline")
(defvar sas-flash-end t
  "*Flash the opening DO or SELECT statement after an 'end;' is typed")
(defvar sas-labels-to-left nil
  "*Buffer-specific: if true then ALL labels are left justified, 
  if false then labels are not moved. 
  Both  sas-labels-to-left and sas-labels-standard may not be true.")
(defvar sas-fast-scan t
  "*If true then top level statements are set to sas-initial-indent,
  otherwise they are indented as the first indented statement is.")
(defvar sas-continue-to-indent nil
  "*For continued statements, indent each new line sas-continued-statement-offset more than the previous")
(defvar sas-minimize-indent nil
  "*If true then use real beginning of statement that contains 
    the DO or SELECT, otherwise use the line the DO or SELECT is on")
;;; --------- other vars ---------------

(defvar sas-indent-grps "" 
  "*statements that change indent level")

(setq sas-indent-grps
      "\\([% \t]do[ \n\t;]\\)\\|\\([ \t]+select[\(; \n\t]\\)\\|\\([% \t]end[ \t]*;\\)")

(defvar sas-string-delimiter "")
(setq sas-string-delimiter "\\([\'\"]\\)" )

(defvar sas-comment-end "" "end of a comment indicator")
(setq  sas-comment-end "\\(\\*/\\)" )
(setq sas-comment-begin "\\(/\\*\\)" )
(defvar sas-comments (concat sas-comment-begin "\\|" sas-comment-end))

(defvar sas-preamble-term ""
  "any label or data or proc statement terminates preamble, a macro 
    is only a temporary suspension")
(setq sas-preamble-term 
    "\\(^\\w+:\\|^proc[ \t\n]\\|^data[ \t\n]\\)\\|\\(^%macro[ \t\n]\\)")

(defvar macro-ender "^%mend[ \t\n;]"
  "this terminates a macro def")
(defvar sas-top-level-labels ""
  "*labels that are ALLWAYS at top level. \n
   Now it is possible that some people may be wierd and make some of them not
   at top level. They deserve NOT to have editing support.")

(setq sas-top-level-labels 
      "\\(^proc[ \t\n]\\|^data[ \t\n]\\|^getrow:\\|^putrow:\\|^init:\\|^main:\\|^term:\\)")

(defvar sas-stmt-left
  ""
  "these statements should be left justified")
(setq sas-stmt-left   
      "entry[ \t\n]\\|%macro[ \t\n]\\|%mend[ \t\n;]\\|^proc[ \t\n]\\|^data[ \n\t;]")
   
(defvar sas-back-scan-regexp ""
  "regions : do - 1; select - 2; end - 3; string - 4; comment-end - 5 ; comment-beg - 6; top-level-labels - 7; left-just-label - 8")

(defvar left-just-label "\\(^\\w+:\\)")

(setq sas-back-scan-regexp
  (concat sas-indent-grps "\\|" sas-string-delimiter  "\\|"
	  sas-comment-end "\\|" sas-comment-begin 
	  "\\|" sas-top-level-labels "\\|" left-just-label))


(defvar sas-comments (concat sas-comment-begin "\\|" sas-comment-end))

(defvar sas-first-skippers ""
  "skip over comments, %commands, entry and length and labels")
(setq sas-first-skippers
      (concat "/\\*\\|^%\\|\\(\\w*:\\)" sas-stmt-left))

(defvar sas-string-reg1  (concat sas-comments "\\|\\('\\)")
  "for string scanning")
(defvar sas-string-reg2 (concat sas-comments "\\|\\(""\\)"))

(defvar sas-cards "" "reg exp for see if we have a cards statement
     region 2 tells us if we need 1 or 4 semis")
(setq sas-cards
  "\\(cards\\|lines\\|datalines\\)\\(\\(4\\)\\|\\)[; \t]")
(defvar sas-cards-end1 "^;[ \t\n]")
(defvar sas-cards-end2 "^;;;;[ \t\n]")

(defvar sas-mode-abbrev-table nil
  "Abbrev table in use in sas-mode buffers.")
(define-abbrev-table 'sas-mode-abbrev-table ())

(defvar sas-mode-map ()
  "Keymap used in SAS mode.")
(if sas-mode-map
    ()
  (setq sas-mode-map (make-sparse-keymap))
  (define-key sas-mode-map ";" 'electric-sas-semi)
  (define-key sas-mode-map "\e\C-q" 'sas-indent-region)
  (define-key sas-mode-map "\177" 'backward-delete-char-untabify)
  (define-key sas-mode-map "\e(" 'sas-flash-open)
  (define-key sas-mode-map "\t" 'sas-indent-command))


(defvar sas-mode-syntax-table nil
  "Syntax table in use in SAS-mode buffers.")

(if sas-mode-syntax-table
    ()
  (setq sas-mode-syntax-table (make-syntax-table))
  (modify-syntax-entry ?_ "w" sas-mode-syntax-table)
  (modify-syntax-entry ?\\ "\\" sas-mode-syntax-table)
  (modify-syntax-entry ?/ ". 14" sas-mode-syntax-table)
  (modify-syntax-entry ?* ". 23" sas-mode-syntax-table)
  (modify-syntax-entry ?+ "." sas-mode-syntax-table)
  (modify-syntax-entry ?- "." sas-mode-syntax-table)
  (modify-syntax-entry ?= "." sas-mode-syntax-table)
  (modify-syntax-entry ?% "." sas-mode-syntax-table)
  (modify-syntax-entry ?< "." sas-mode-syntax-table)
  (modify-syntax-entry ?> "." sas-mode-syntax-table)
  (modify-syntax-entry ?& "." sas-mode-syntax-table)
  (modify-syntax-entry ?| "." sas-mode-syntax-table)
  (modify-syntax-entry ?\; "w" sas-mode-syntax-table) 
  ;; above is a hack to make forward-sexp do what I want
  (modify-syntax-entry ?\' "\"" sas-mode-syntax-table))

(defvar sas-moded-once nil 
  "for doing things to the syntax table and keymap only once")

(defun sas-mode ()
  "Major mode for editing SAS code.
  Tab indents for SAS code.
  Paragraphs are separated by blank lines only.
  Numeric-pad shift 5 - sas-indent-region
  SASfile-header function inserts proper header

  Do not use the comment statement of '* comment ;' , 
  use instead the C style  /* comment */ type.

  It is recommended that the variable sas-labels-standard be true.
  This requires that left justified labels are treated as 'subroutines'.
  This speeds up indenting greatly.  Labels that are not marking a
  subroutine need to be indented by at least 1 space.

  Variables controlling indentation style and use:
 sas-labels-standard 
  Buffer-specific: If true then labels that are left justified are
   considered to be like subroutines, otherwise , use some indent.
   This will speed up the indenting process.  The command 
    sas-labels-toggle-standard will change the toggle the value
   and make sure that sas-labels-to-left is nil.
 sas-undent-end 
  Controls where an end statement is indented to. T makes it indent at the
    do or select, nil indents the end as an inside statement.
 sas-tab-always-indent
    Non-nil means TAB in SAS mode should always reindent the current line,
    regardless of where in the line point is when the TAB command is used.
 sas-electric-semi
    Treat ';' as an indenter command  and maybe add a newline,
    see sas-auto-newline
 sas-flash-end
    Flash the opening DO or SELECT statement after an 'end;' is typed
 sas-auto-newline
    Insert new-line and indent after an electric ';'
 sas-indent-level
    Indentation of SAS statements within surrounding block.
    The surrounding block's indentation is the indentation
    of the line on which the DO or SELECT statement appears.
 sas-initial-indent
    Indent for top level statements
 sas-preamble-indent
    Statements found to be in the preamble are indented this much
 sas-continued-statement-offset
    Extra indentation given to a substatement, such as the
    then-clause of an if or other continued lines.
 sas-labels-to-left
    if true then ALL labels are left justified, if false then
    labels are not moved
 sas-continue-to-indent
    For continued statements, indent each new line
    sas-continued-statement-offset more than the previous
 sas-minimize-indent 
  If true then use real beginning of statement that contains 
    the DO or SELECT, otherwise use the line the DO or SELECT is on.
 sas-mode-hook
    hook to run on start of sas-mode.
 sas-mode-load-hook
    hook that is for key binding"
    
  (interactive)
  (kill-all-local-variables)
  (use-local-map sas-mode-map)
  (setq major-mode 'sas-mode)
  (setq mode-name "SAS")
  (setq local-abbrev-table sas-mode-abbrev-table)
  (set-syntax-table sas-mode-syntax-table)
  (make-local-variable 'paragraph-start)
  (setq paragraph-start (concat "^$\\|" page-delimiter))
  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate paragraph-start)
  (make-local-variable 'paragraph-ignore-fill-prefix)
  (setq paragraph-ignore-fill-prefix t)
  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'sas-indent-line)
  (make-local-variable 'require-final-newline)
  (setq require-final-newline t)
  (make-local-variable 'comment-start)
  (setq comment-start "/* ")
  (make-local-variable 'comment-end)
  (setq comment-end " */")
  (make-local-variable 'comment-column)
  (setq comment-column 32)
  (make-local-variable 'comment-start-skip)
  (setq comment-start-skip "/\\*+ *")
  (make-local-variable 'comment-indent-hook)
  (setq comment-indent-hook 'sas-comment-indent)
  (make-local-variable 'parse-sexp-ignore-comments)
  (setq parse-sexp-ignore-comments t)
  (if (not sas-moded-once )
      (progn 
	(run-hooks 'sas-mode-load-hook)
	(setq sas-moded-once t)))
  (run-hooks 'sas-mode-hook))

(defun sas-comment-indent ()
  (if (looking-at "^/\\*")
      0				;Existing comment at bol stays there.
    (save-excursion
      (skip-chars-backward " \t")
      (max (1+ (current-column))	;Else indent at comment column
	   comment-column))))	; except leave at least one space.


(defun electric-sas-semi (arg)
  "Insert character and correct line's indentation if not on column 1"
  (interactive "P")
  (if (and sas-electric-semi 
	   (not (= 0 (current-column)))
	   (not (eq (preceding-char) ?\;)))
      (electric-sas-terminator arg)
    (self-insert-command (prefix-numeric-value arg))))

(defun electric-sas-terminator (arg)
  "Insert character and correct line's indentation."
  (interactive "P")
  (let (insertpos
	 in-string
	 (end (point)))
    (if (and (not arg) (eolp)
	     (= 1 (prefix-numeric-value arg))
	     (not (save-excursion
		    (beginning-of-line)
		    (skip-chars-forward " \t")
		    ;; check if we are in a string
		    ;;  assume strings can only be on one line
		    (while (re-search-forward sas-string-delimiter end t)
			   (setq in-string t)
			   (if (re-search-forward (char-to-string (preceding-char))
				 end t)
			     (setq in-string nil)))
		    in-string)))
      (progn
	(insert last-command-char)
	(sas-indent-line)
	(if sas-flash-end
	  (save-excursion
	    (forward-word -1)
	    (if (looking-at "end;")
	      (sas-flash-open))))
	(and sas-auto-newline
	     (progn
	       (newline)
	       (sas-indent-line)))
	)
      (self-insert-command (prefix-numeric-value arg)))))

(defvar sas-grouper-helper t 
  "nil if not in a do select nest but sas-backward-balanced-grouper returns a number")

(defun sas-backward-balanced-grouper (start )
  "finds the containing SAS do or select statement, returns that pos or t if in a comment or nil if not contained"
  (save-excursion
    (let ((in-comment nil)
	  (l-num (1+ (count-lines 1 (point))))
	  (in-string nil)
	  (groupend-count 0)
	  (groupbeg-count 0))
      (catch 'niler
	(while t
          (if (re-search-backward sas-back-scan-regexp start t)
              (progn
                (cond ((match-beginning 1) ; a do statement
                       (setq groupend-count (1- groupend-count)))
                      ((match-beginning 2) ; a select
                       ;; need to determine if a statement or a function
                       ;; assume if an equal 
                       (save-excursion
                         ;; can speed up here by not using re-search and
                         ;; looking-at. Do not use tabs.
                         (skip-chars-backward " \t")
                         (if (not (or (char-equal (preceding-char) ?=)
                                      (char-equal (preceding-char) ?\()))
                             (setq groupend-count (1- groupend-count)))))
                      ((match-beginning 3)
                       (setq groupend-count (1+ groupend-count)))
                      ((match-beginning 4)
                       ;; assume we are not in a string to begin with
                       (back-over-sas-string start)
                       )
                      ((match-beginning 5)
                       ;; assume we are not in a comment to begin with.
                       (back-over-comment start))
                      ((match-beginning 6)
                       ;; uh-oh - we were in a comment!
                       (throw 'niler t))
                      ((or (match-beginning 7)
                           (and sas-labels-standard (match-beginning 8)))
                       ;; at a top level label!
                       ;; check if we are nested.
                       (if (> groupend-count 0)
                           ;; we have an end with no beginning
                           (error "Line: %d; A top level label \"%s\" at line %d found above an unmatched end statement"
                                  l-num
                                  (buffer-substring
                                    (or (match-beginning 7)
                                        (match-beginning 8))
                                    (or (match-end 7)
                                        (match-end 8)))
                                  (1+ (count-lines 1 (point))))
                           (setq  sas-grouper-helper nil)
                           (throw 'niler (point)))
                       )
                      ;; if not sas-labels-standard (match-beginning 8)
                      ;;  do nothing
                      ;; need if's done.
                      )
                (if (< groupend-count 0)
                    (progn
                      (setq  sas-grouper-helper t)
                      (throw 'niler (point)))))
              (throw 'niler nil)))))))

;; new for speed 	
(defun back-over-sas-string (start)
  "back over SAS strings, may start with single or double and have opposite embedded, unless bol or comment start is found"
  (if (not (search-backward
            (if (eq (following-char) ?\')
                "'"  "\"")
            nil t))
      (error "Error: possible unterminated string, Line: %d" 
             (1+ (count-lines 1 (point))))))
      


;; modified for faster operation. 
(defun back-over-comment (start)
  "if we are in a comment - back up to start of comment, if -not in comment , no back up"
  ;; can spped up here by string scearch for only comment start '/*'
  (search-backward "/*" start t))

(defun in-sas-comment-p (start)
  "are we in a comment"
  ;; later may want to make sure we are not in a string here
  (save-excursion
    (and (re-search-backward sas-comments start t)
         (match-beginning 1))))
          

(defun sas-current-indentation ()
  "get indentation of current line"
  (end-of-line)
  (let ((eol (point)))
    (beginning-of-line)
    (if (re-search-forward "^ *\\w*:[ \t]*" eol t)
      ;; we have a label, return current col
      (current-column)
      (current-indentation))))
      

(defun sas-calculate-indent (parse-start)
  "calculate the indent for a sas line, return a number or t if in a comment, later check if in a string, since strings can only be on one line"
  (save-excursion
    (let ((in-comment (in-sas-comment-p (point-min))))
      (if in-comment 
          (progn
            (goto-char (1+ in-comment))
            (current-column))
          (progn
            (beginning-of-line)
            ;; take care of the stupid /* not in column 1 thing.
            (if (and (not (eobp)) (char-equal (char-after (point)) 32) )
                (forward-char 1))
            (if  (looking-at "/\\*")  1 
                (skip-chars-forward " \t")
                (if (looking-at sas-stmt-left)
                    0
                    
                    (let* ((indent-point (point))
                           (case-fold-search nil)
                           state
                           (limitr  (or parse-start (point-min)))
                           (containing-open 
                            (if (and sas-undent-end (looking-at "end[ ;]"))
                                (save-excursion
                                  ;(debug)
                                  (end-of-line 1)
                                  (sas-backward-balanced-grouper limitr))
                                (sas-backward-balanced-grouper limitr)))
                           (limit (or containing-open parse-start (point-min)))
                           containing-sexp)
                      (if (eq containing-open t)
                          ;; we are in a comment
                          t
                          (goto-char limit)
                          (while (< (point) indent-point)
                            (setq containing-sexp
                                  (car (cdr (parse-partial-sexp
                                              (point) indent-point 0)))))
                          (cond 
                            ((eq containing-open t)
                             ;; we are in a comment
                             t)
                            ((not (null containing-sexp))
                             ;; we are in an expression
                             (goto-char (1+ containing-sexp))
                             (current-column))
                            (t
                             ;; Statement level.  Is it a continuation or a new statement?
                             ;; Find previous non-comment character.
                             (goto-char indent-point)
                             (sas-backward-to-noncomment limitr)
	      
                             (while (eq (preceding-char) ?\,)
                               (sas-backward-to-start-of-continued-exp limitr)
                               (beginning-of-line)
                               (sas-backward-to-noncomment limitr))
	      
                             ;; Now we get the answer.
                             (if (not (memq (preceding-char) '(nil ?\, ?\; ?\:)))
                                 ;; This line is continuation of preceding line's statement;
                                 ;; indent  sas-continued-statement-offset  more than the
                                 ;; previous line of the statement.
                                 (progn
                                   (if sas-continue-to-indent
                                       (sas-backward-to-start-of-continued-exp limit)
                                       (sas-backward-to-real-start-of-continued-exp
                                         (or containing-sexp (point))
                                         limit))
                                   (+ sas-continued-statement-offset (sas-current-indentation)
                                      ))
                                 ;; This line starts a new statement.
                                 ;; Position following last unclosed open.(DO or SELECT)
                                 (if (and containing-open sas-grouper-helper)
                                     (progn ; containing-open is position of opening 
                                        ;(debug)
                                       (goto-char containing-open)
                                       (if sas-minimize-indent
                                           (sas-backward-to-real-start-of-continued-exp
                                             (point)
                                             1)
                                           (beginning-of-line))
                                       ;;skip labels and white space
                                       (if (> containing-open (point))
                                           (re-search-forward "^ *\\w*:" 
                                                              containing-open t))
                                       (skip-chars-forward " \t\n")
                                       (+ (current-column) sas-indent-level))
                                     ;; we are top level, find if we are in the preamble
                                     (save-excursion
                                       (goto-char (point-min))
                                       (if (sas-in-preamble indent-point)
                                        ; in preamble proper
                                           sas-preamble-indent
                                        ; else in main part or macro def
                                           sas-initial-indent)))))))))))))))


(defun sas-backward-to-noncomment (lim)
  (let (opoint stop)
    (while (not stop)
      (skip-chars-backward " \t\n\f" lim)
      (setq opoint (point))
      (if (and (>= (point) (+ 2 lim))
	       (save-excursion
		 (forward-char -2)
		 (looking-at "\\*/")))
	  (search-backward "/*" lim 'move)
	(beginning-of-line)
	(skip-chars-forward " \t")
	(setq stop t)
	(if stop (goto-char opoint)
	  (beginning-of-line))))))

(defun sas-backward-to-real-start-of-continued-exp (start lim)
  "go back to a real semi or full colon, start must not be embedded in a
  open paren"
  (goto-char start)
  (let (opoint stop)
    (while (not  (or (looking-at "\\(^\\w+\\:\\)\\|\\(\\w*\\;\\)")
		     (bobp)
		     (> lim (point))))
	   (forward-sexp -1))
    ; we are at a label beginning or a semi or at the limit.
    (forward-sexp (if  (> lim (point)) 1 2 ))
    (forward-word -1); we should be there!
    ))
	

(defun sas-backward-to-start-of-continued-exp (lim)
  (if (= (preceding-char) ?\))
      (forward-sexp -1))
  (beginning-of-line)
  (if (<= (point) lim)
      (goto-char (1+ lim)))
  (skip-chars-forward " \t"))

(defun sas-backward-to-start-of-if (&optional limit)
  "Move to the start of the last ``unbalanced'' if."
  (or limit (setq limit (save-excursion (beginning-of-defun) (point))))
  (let ((if-level 1)
	(case-fold-search nil))
    (while (not (zerop if-level))
      (backward-sexp 1)
      (cond ((looking-at "else\\b")
	     (setq if-level (1+ if-level)))
	    ((looking-at "if\\b")
	     (setq if-level (1- if-level)))
	    ((< (point) limit)
	     (setq if-level 0)
	     (goto-char limit))))))


(defun sas-in-preamble (place)
  "point is at beginning usally, if PLACE is still in preamble  but not in a macro def, return t, else return nil"
  (catch 'pre-amb-term
    (while (re-search-forward sas-preamble-term place t)
	   (cond  ((match-beginning 1) ; a real terminator
		   (throw 'pre-amb-term nil))
		  ((match-beginning 2) ; a macro def
		   (if (not (re-search-forward macro-ender place t))
		     ;; we are in a macro def
		     (throw 'pre-amb-term nil))))
	   )
    t))



    
(defun calculate-sas-indent-within-comment ()
  "Return the indentation amount for line, assuming that
the current line is to be regarded as part of a block comment."
  (let (end star-start)
    (save-excursion
      (beginning-of-line)
      (skip-chars-forward " \t")
      (setq star-start (= (following-char) ?\*))
      (skip-chars-backward " \t\n")
      (setq end (point))
      (beginning-of-line)
      (skip-chars-forward " \t")
      (and (re-search-forward "/\\*[ \t]*" end t)
	   star-start
	   (goto-char (1+ (match-beginning 0))))
      (current-column))))

(defun sas-indent-line ()
  "Indent current line as sas/scl code. Return the amount the indentation changed by."
  (let ((indent (sas-calculate-indent nil))
	beg shift-amt label
	(case-fold-search nil)
	(pos (- (point-max) (point))))
    (beginning-of-line)
    (setq beg (point))
    (cond ((eq indent nil)
	   (setq indent (sas-current-indentation)))
	  ((eq indent t)         
	   (setq indent (calculate-sas-indent-within-comment)))
	  ;; macro calls ??
	  ((looking-at "[ \t]*%macro;")
	   (setq indent 0))
	  ((looking-at "^%")
	   (setq indent 0))
	  ;; here we have skipped spaces and tabs now
	  ((and
	    (skip-chars-forward " \t")
	    (looking-at "else\\b"))
	   (setq indent (save-excursion
			  (sas-backward-to-start-of-if)
			  (sas-current-indentation))))
          ((looking-at "\\w*:")
           (setq indent (if sas-labels-to-left 
                            0  (current-indentation))))
          )
    (setq shift-amt (- indent (current-column)))
    (if (zerop shift-amt)
	(if (> (- (point-max) pos) (point))
	    (goto-char (- (point-max) pos)))
	(delete-horizontal-space)
	(indent-to indent)
	;; If initial point was within line's indentation,
	;; position after the indentation.  Else stay at same point in text.
	(if (> (- (point-max) pos) (point))
	    (goto-char (- (point-max) pos))))
    shift-amt))



(defun sas-indent-command (&optional whole-exp)
  
  "Indent current line as SAS code, or in some cases insert a tab character.
If sas-tab-always-indent is non-nil (the default), always indent current line.
Otherwise, indent the current line only if point is at the left margin
or in the line's indentation; otherwise insert a tab.

A numeric argument, regardless of its value,
means indent rigidly all the lines of the expression starting after point
so that this line becomes properly indented.
The relative indentation among the lines of the expression are preserved."

   (interactive "P")
   (if whole-exp
     ;; If arg, always indent this line as SAS
     ;; and shift remaining lines of expression the same amount.
     (let ((shift-amt (sas-indent-line))
	   beg end)
       (message "%d" shift-amt)
       (save-excursion
	 (if sas-tab-always-indent
	   (beginning-of-line))
	 (setq beg (point))
	 (forward-sexp 1)
	 (setq end (point))
	 (goto-char beg)
	 (forward-line 1)
	 (setq beg (point)))
       (if (> end beg)
	 (indent-code-rigidly beg end shift-amt "#")))
     (if (and (not sas-tab-always-indent)
	      (save-excursion
		(skip-chars-backward " \t")
		(not (bolp))))
       (insert-tab)
       (sas-indent-line))))

;
;



(defun sas-flash-open (&optional start)
  "Flash the opening grouper - Do or Select"
  (interactive) 
  (let ((opener (sas-backward-balanced-grouper (or start (point-min)))))
    (cond ((eq opener t)
	   (message "You are within a comment")
	   (beep 1)
	   )
	  ((eq opener nil)
	   (message "No matching Do or Select found")
	   (beep 1))
	  (t ; have a position, need to see if visable
	   (save-excursion
	     (goto-char opener)
	     (if (or (looking-at sas-top-level-labels)
                     (and sas-labels-standard 
                          (looking-at left-just-label)))
		 (error "A top level label \"%s\" at line %d found above this unmatched end statement"
				     (buffer-substring
				       (match-beginning 0)
				       (match-end 0))
				     (1+ (count-lines 1 (point)))))
	     (if (>= opener (window-start) )
	         ; yes visable
		 (sit-for sas-flash-pause)
		 (message
		   (concat
		     "LINE %d:"
		     (buffer-substring
			    (progn
			      (beginning-of-line)
			      (point))
			    (progn
			      (end-of-line)
			      (point))))
		   (1- (1+ (count-lines 1 (point)))))))))))



    
    


(defun sas-indent-region (parg)
  ;; This currently is a very poor way to do this. It really only needs to
  ;; do simple parsing as it goes forward. Other functions were made and this
  ;; is the easiest. Change later.
    "Indent the sas code from mark to point unless a prefix, then from buffer start to point if a positive prefix, and from point to end if negative"
  (interactive "P")
  (let*
    ((line-cnt 0)
     (arg (prefix-numeric-value parg))
     (start (if (> arg 1)
	      (point-min)
	      (min (point) (mark))))
     (start-line (save-excursion
		   (goto-char start)
		   (1+ (count-lines 1 (point)))))
     (end   (if (> 0 arg)
	      (point-max)
	      (max (point) (mark))))
     (end-pos (- (point-max) end))
     (end-line (save-excursion
		 (goto-char end)
		 (1+ (count-lines 1 (point)))))
     (num-lines (1+ (- end-line start-line))))
    (message "indenting SAS code region, ....")
    (save-excursion
      (goto-char start)
      (beginning-of-line)
      (catch 'at-eob
	(while  (> (- (point-max) (point))
		   end-pos)
	  (setq num-lines (1- num-lines))
	  (setq line-cnt (1+ line-cnt))
	  (if (> line-cnt 5)
	      (progn 
		(message "indenting SAS line %d" (1+ (count-lines 1 (point))))
		(setq line-cnt 1)))
	  (let ((bol (point)))
	    (beginning-of-line)
	    (skip-chars-forward " \t")
	    (cond ((eolp) ; blank line, delete spaces
		   (delete-region bol (point))
		   (forward-line))
		  ((looking-at sas-cards)
		   ;; beginning of SAS cards or data lines
		   (if  (re-search-forward 
			      (if (match-beginning 3) 
				  sas-cards-end2
				  sas-cards-end1)
			      nil t)
			(forward-line)
			(looking-at sas-cards)
			(error 
			  "A %s statement at line %d not ended correctly"
			  (buffer-substring (match-beginning 1)
					    (match-end 1))
			  (1+ (count-lines 1 (point))))))
		  (t (sas-indent-line)
		     (forward-line)))
	    (if  (= (point) (point-max))
		(throw 'at-eob nil))
	    )))
      (beep)
      (message "Done indenting!"))))

(defun sas-labels-toggle-standard ()
 "toggle sas standard mode, ie the sas-labels-standard buffer specfic 
  variable"
  (interactive)
  (setq sas-labels-to-left nil)
  (setq sas-labels-standard (not sas-labels-standard))
  (message (if sas-labels-standard 
               "New SAS standard, indent non-subroutine labels"
               "Labels have no standard for indent"))
  )



(defun sas-big-pic (col)
  "Do a set-selctive-display at the current column, Column 1 resets,
   this hides all lines that are indented further that the column"
  (interactive (list (current-column)))
  (set-selective-display col))

(defun sas-forward-function()
  "SAS does not have strong markers. So go forward to a label, left justified"
  (interactive)
  (if (not (re-search-forward left-just-label nil t))
      (message "No more left justified labels found")))

(defun sas-backward-function()
  "SAS does not have strong markers. So go backward to a label, left justified"
   (interactive)
   (if (not (re-search-backward left-just-label nil t))
      (message "No more left justified labels found")))

(provide 'sas-mode)


